package tso.tnative;

class TLinuxPrinter {	
static {
	try {
		System.load("/home/tsrj/work/libtlinuxprinter.so");
        } catch (UnsatisfiedLinkError e) {
            System.out.println("load:" + e);
        }
}
public static native String[] getPrinterNames();
public static native int printPDFFile(String printerName, String pdfFilePath);
public static void main(String[] argv) {
	String[] printerNames = TLinuxPrinter.getPrinterNames();
	if (printerNames == null) {
		System.out.println("it hasn't printer");	
	} else {
		int len = printerNames.length;
		for(int i=0; i<len; i++) {
			System.out.println("printer is:" + printerNames[i]);
		}
	}

    TLinuxPrinter.printPDFFile("HP_LaserJet_M101-M106_USB_VNF3F10203_HPLIP", "/home/tsrj/work/test.pdf");
}
}
