#include <vector>
#include <string>
#include <string.h>
#include <stdio.h> 
#include <cups/cups.h>   

typedef struct
{
    int num_dests;
    cups_dest_t *dests;
} MyUserData;

static int destinationCallback(MyUserData *user_data, unsigned flags, cups_dest_t * dest)
{
    if (flags & CUPS_DEST_FLAGS_REMOVED)
    {
        user_data->num_dests = cupsRemoveDest(dest->name, dest->instance, user_data->num_dests, &(user_data->dests));
    }

    else
    {
        user_data->num_dests = cupsCopyDest(dest, user_data->num_dests, &(user_data->dests)); //TODO:test:more printer
    }
    return (1);
}

static int getDestinations(cups_ptype_t type, cups_ptype_t mask, cups_dest_t **dests)
{
    MyUserData user_data = { 0, NULL };
    cupsEnumDests(CUPS_DEST_FLAGS_NONE, 1000, NULL, type, mask, (cups_dest_cb_t)destinationCallback, &user_data);
    
    if (user_data.num_dests == 0)
        return 0;
    
    *dests = user_data.dests;
    return (user_data.num_dests);
}

//find printer by printer name
static cups_dest_t * findPrinterByName(const char * printName){
    cups_dest_t * destArr = 0; //TODO:release cups_dest_t
    if (printName == 0) {
        return 0;
    }

    int numberOfPrinter = getDestinations(0, 0, &destArr);
    if (numberOfPrinter <=0 || destArr == 0){
        return 0;
    }

    for (int i=0; i<numberOfPrinter; i++) {
        if (strcmp(destArr[i].name, printName) == 0) {
            return &destArr[i]; //TODO:release cups_dest
        } 
    }

    return 0;
}

static int createPrintJob(cups_dest_t * dest) {
    int job_id = 0;
    int num_options = 0;
    cups_option_t *options = NULL;
    cups_dinfo_t *info = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, dest);

    num_options = cupsAddOption(CUPS_COPIES, "1",num_options, &options);
    num_options = cupsAddOption(CUPS_MEDIA, CUPS_MEDIA_LETTER, num_options, &options);
    num_options = cupsAddOption(CUPS_SIDES, CUPS_SIDES_TWO_SIDED_PORTRAIT, num_options, &options);

    if (cupsCreateDestJob(CUPS_HTTP_DEFAULT, dest, info, &job_id, "My Document", num_options, options) == IPP_STATUS_OK) {
        //printf("Created job: %d\n", job_id); //TODO:log
        return job_id;
    }
    //else
    //    printf("Unable to create job: %s\n", cupsLastErrorString()); //TODO:log

    return -1;
}

static int printPDF(cups_dest_t * dest, int job_id, const char * pdfFilePath) {
    FILE *fp = fopen(pdfFilePath, "rb");
    if (!fp) {
        fclose(fp); //???crash
        return -1;
    }

    size_t bytes = 0;
    char buffer[1024];
    cups_dinfo_t * info = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, dest);
    int status = 1;

    if (cupsStartDestDocument(CUPS_HTTP_DEFAULT, dest, info, job_id, pdfFilePath, CUPS_FORMAT_PDF, 0, NULL, 1) == HTTP_STATUS_CONTINUE){
        while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
            if (cupsWriteRequestData(CUPS_HTTP_DEFAULT, buffer, bytes) != HTTP_STATUS_CONTINUE)
                break; //???fail
    }

    if (cupsFinishDestDocument(CUPS_HTTP_DEFAULT, dest, info) == IPP_STATUS_OK) {
        //puts("Document send succeeded."); //TODO:log
    }
    else {
        //printf("Document send failed: %s\n", cupsLastErrorString()); //TODO:log
        status = -2;
    }
    
    fclose(fp);
    return status;
}

int TIO_Print_GetNamesOfPrinter(std::vector<std::string> & namesOut) {
    cups_dest_t * destArr = 0; //TODO:release cups_dest_t
    int numberOfPrinter = getDestinations(0, 0, &destArr);
    if (numberOfPrinter <=0 || destArr == 0){
        return 0;
    }

    for (int i=0; i<numberOfPrinter; i++) {
        std::string nameStr(destArr[i].name);
        namesOut.push_back(nameStr);    
    }
    
    return 1;
}

//block when print file ???
int TIO_Print_PrintPDFFile(const char * printName, const char * pdfFilePath) {
    cups_dest_t * dest = findPrinterByName(printName);
    if (dest == 0) {
        return -1;
    }

    int jobID = createPrintJob(dest);
    if (jobID<0) {
        return -2;
    }

    if (printPDF(dest, jobID, pdfFilePath) < 0 ) {
        return -3; 
    }

    return 1;
}

/*
int main(){
	std::vector<std::string> names;
	TIO_Print_GetNamesOfPrinter(names);
	
	int count = names.size();
	for (int i=0; i<count; i++) {
		printf("name = %s\n", names[i].c_str());	
	}

    //VirtualPrinterTA
    cups_dest_t * dest = findPrinterByName("VirtualPrinterTA");
    if (dest) {
        printf("find %s printer\n", dest->name);
    } else {
        printf("hasn't find VirtualPrinterTA printer\n");
    }
	return 0;
}
*/
