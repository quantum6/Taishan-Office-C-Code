#include "tclipboard.h"

Bool getSelectionData(Display *display, const Window window, const char *bufName, const char *fmtName,  const char * outputFile, vector<string>* pvecFormats)
{
	char *result;
	unsigned long ressize=0, restail=0, total=0;
	int resbits=0;
	FILE* fp = NULL;

	char propName[128] = {0};
	sprintf(propName, "%s", "XSEL_DATA");
	Atom  bufid = XInternAtom(display, bufName,  False),
		  fmtid = XInternAtom(display, fmtName,  False),
		 propid = XInternAtom(display, propName, False),
		 incrid = XInternAtom(display, "INCR",   False);

	XEvent event;
	XSelectInput (display, window, PropertyChangeMask);
	XConvertSelection(display, bufid, fmtid, propid, window, CurrentTime);

	do 
	{
		XNextEvent(display, &event);
	} while (event.type != SelectionNotify || event.xselection.selection != bufid);

	if (outputFile && strlen(outputFile) > 0)
    {
		fp = fopen(outputFile, "wb");
    }
	
	if (event.xselection.property)
	{
		XGetWindowProperty(display, window, propid, 0, LONG_MAX/4, True, AnyPropertyType, &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);

		if (fmtid != incrid && ressize > 0) 
		{
			if ((strcmp(fmtName, "TARGETS") == 0) && pvecFormats)
			{
				Atom* pAtom = (Atom*)result;
				for (int i=0; i<ressize; ++i)
				{
					char* atomName = XGetAtomName(display,pAtom[i]);					
					pvecFormats->push_back(atomName);
				}
			}		 
			else if (fp)
			{
				fwrite(result,1,ressize,fp);
				fclose(fp);
				fp = NULL;
			}

			XFree(result);
		}
		else if(ressize > 0)
		{
			do 
			{
				do 
				{
					XNextEvent(display, &event);
				} while (event.type != PropertyNotify || event.xproperty.atom != propid || event.xproperty.state != PropertyNewValue);

				XGetWindowProperty(display, window, propid, 0, LONG_MAX/4, True, AnyPropertyType,
					&fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);

				if (ressize > 0 && fp)
				{
					fwrite(result,1,ressize,fp);
				}
				XFree(result);
			} while (ressize > 0);
			
			if (fp)
			{
				fclose(fp);
				fp = NULL;
			}
		return True;
		}

	}
    return False;
}

bool getSelectionDataToFile(const char* fmtName, char* outputFile)
{
	bool bResult = false;
	if (fmtName==NULL || strlen(fmtName)==0 || outputFile==NULL || strlen(outputFile)==0 )
    {
		return bResult;
    }

	Display *display = XOpenDisplay(NULL);
	unsigned long color = BlackPixel(display, DefaultScreen(display));
	Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,0, 1,1, 0, color, color);
	
	Bool result = getSelectionData(display, window, CLIPBOARD_SELECTION_NAME, fmtName, outputFile,NULL);
	if (result)
	{
		// check file exist and size
		if ((0 == access(outputFile, F_OK)) && getFileSize(outputFile) > 0)
		{
			bResult = true;
		}
	}
	
	XDestroyWindow(display, window);
	XCloseDisplay(display);
	return bResult;
}

Bool getTargets(vector<string>& vecFormats)
{
	Display *display = XOpenDisplay(NULL);
	unsigned long color = BlackPixel(display, DefaultScreen(display));
	Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,0, 1,1, 0, color, color);

	Bool result = getSelectionData(display, window, CLIPBOARD_SELECTION_NAME, TARGETS_NAME, NULL, &vecFormats);

	XDestroyWindow(display, window);
	XCloseDisplay(display);
	return result;
}

int getFileSize(const char* file_name)
{
	if (file_name==NULL || strlen(file_name)==0)
    {
		return -1;
    }
	
    struct stat statbuff;
    stat(file_name, &statbuff);
    return (int)(statbuff.st_size);
}
