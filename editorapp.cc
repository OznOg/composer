#include <QtGui> 
#include <cstdlib>
#include "editorapp.hh"


EditorApp::EditorApp(QWidget *parent): QMainWindow(parent)
{
	ui.setupUi(this);
	// Now adjust the NoteGraph's width according to the content
	ui.noteGraph->updateWidth();
}

void EditorApp::on_actionAbout_triggered()
{
	QMessageBox::about(this, "About",
		"Semi-automatic karaoke song editor.\n"
		);
}

void EditorApp::on_actionExit_triggered()
{
	exit(0); // Hack
}