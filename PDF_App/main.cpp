#include "MyForm.h"
#include "ConeccionHTTP.h"

using namespace System;
using namespace System::Windows::Forms;

using namespace PDFApp;

[STAThreadAttribute]
int main(array<String^>^ args) {
	// Para la interfaz
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	
	// Conectar al Server
	ConeccionHTTP conn;
	conn.run();

	PDFApp::MyForm^ form = gcnew PDFApp::MyForm();

	Application::Run(form);

	return 0;
}

//hola