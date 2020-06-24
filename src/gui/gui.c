#include <gtk/gtk.h>
#include "ijvm.h"
#include "state.h"
#include <string.h>
#include <stdio.h>

extern ijvm_t machine;
extern char GUIbuffer[50000];

GtkWidget *window;
GtkWidget *table;
GtkWidget *Start;
GtkWidget *Reset;
GtkWidget *Step;
GtkWidget *Run;

GtkWidget *outputLabel;
GtkWidget *output;
GtkWidget *outputScroll;
GtkTextBuffer *buffer;
GtkTextIter iter;

GtkWidget *pcLabel;
GtkWidget *pc;
GtkTextBuffer *pcBuffer;
GtkTextIter pcIter;

GtkWidget *ToSLabel;
GtkWidget *ToS;
GtkTextBuffer *ToSBuffer;
GtkTextIter ToSIter;

GtkWidget *file_dir;
GtkWidget *chooseButton;

GtkWidget *file_inp;
GtkWidget *chooseInp;

GtkWidget *file_out;
GtkWidget *chooseOut;

char* itoa(int val, int base){
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];
}

void getFileDir(GtkWidget *widget, gpointer data){
	gtk_entry_set_text((GtkEntry*)file_dir, gtk_file_selection_get_filename(GTK_FILE_SELECTION (data)));
  	gtk_widget_destroy(data);
}

void choose(GtkWidget *widget, gpointer data){
	GtkWidget *file_explorer;
	file_explorer = gtk_file_selection_new("Choose an IJVM binary");
	gtk_widget_show(file_explorer);
	g_signal_connect (GTK_FILE_SELECTION(file_explorer)->ok_button, "clicked", G_CALLBACK(getFileDir), file_explorer);
    g_signal_connect_swapped (GTK_FILE_SELECTION (file_explorer)->cancel_button, "clicked", G_CALLBACK(gtk_widget_destroy), file_explorer);
}

void getInpDir(GtkWidget *widget, gpointer data){
	gtk_entry_set_text((GtkEntry*)file_inp, gtk_file_selection_get_filename(GTK_FILE_SELECTION (data)));
  	gtk_widget_destroy(data);
}

void choose_inp(GtkWidget *widget, gpointer data){
	GtkWidget *file_explorer;
	file_explorer = gtk_file_selection_new("Choose an input file");
	gtk_widget_show(file_explorer);
	g_signal_connect (GTK_FILE_SELECTION(file_explorer)->ok_button, "clicked", G_CALLBACK(getInpDir), file_explorer);
    g_signal_connect_swapped (GTK_FILE_SELECTION (file_explorer)->cancel_button, "clicked", G_CALLBACK(gtk_widget_destroy), file_explorer);
}

void getOutDir(GtkWidget *widget, gpointer data){
	gtk_entry_set_text((GtkEntry*)file_out, gtk_file_selection_get_filename(GTK_FILE_SELECTION (data)));
  	gtk_widget_destroy(data);
}

void choose_out(GtkWidget *widget, gpointer data){
	GtkWidget *file_explorer;
	file_explorer = gtk_file_selection_new("Choose an output file");
	gtk_widget_show(file_explorer);
	g_signal_connect (GTK_FILE_SELECTION(file_explorer)->ok_button, "clicked", G_CALLBACK(getOutDir), file_explorer);
    g_signal_connect_swapped (GTK_FILE_SELECTION (file_explorer)->cancel_button, "clicked", G_CALLBACK(gtk_widget_destroy), file_explorer);
}

void start(GtkWidget *widget, gpointer data){
	char* binary_file = (char*)gtk_entry_get_text((GtkEntry*)file_dir);
	init_ijvm(binary_file);
	char* inp_dir = (char*)gtk_entry_get_text((GtkEntry*)file_inp);
	char* out_dir = (char*)gtk_entry_get_text((GtkEntry*)file_out);
	FILE *inp, *out;
	if((inp = fopen(inp_dir, "rb"))) set_input(inp);
	if((out = fopen(out_dir, "wb"))) set_output(out);
	char *msg = "INVOKE init_ijvm()\n";
	gtk_text_buffer_set_text(buffer, msg, -1);
}

void reset(GtkWidget *widget, gpointer data){
	if(machine.isRunning){
		destroy_ijvm();
		char *msg = "IJVM destroyed\n";
		gtk_text_buffer_set_text(buffer, msg, -1);
	}
}

void runProgram(GtkWidget *widget, gpointer data){
	run();
	gtk_text_buffer_set_text(buffer, GUIbuffer, -1);
	fflush(machine.out);
}

void nextState(GtkWidget *widget, gpointer data){
	if(machine.pc + 1 < machine.text_size){
		step();
		char *pcString = itoa(machine.pc, 10);
		char *c = "/";
		char str[50]={0};
		strncat(str, pcString, 20);
		strncat(str, c, 2);
		char *pcMax = itoa(machine.text_size, 10);
		strncat(str, pcMax, 20);
		gtk_text_buffer_set_text(pcBuffer, str, -1);
		
		//show ToS
		char *ToS_value = itoa(tos(), 10);
		gtk_text_buffer_set_text(ToSBuffer, ToS_value, -1);

		//show output
		gtk_text_buffer_set_text(buffer, GUIbuffer, -1);
	}
}

int main(int argc, char *argv[]) {

	gtk_init(&argc, &argv);

	// Main layout
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Integer Java virtual machine - GUI");
	gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);
	
	table = gtk_table_new(9, 6, TRUE);
  	gtk_container_add(GTK_CONTAINER(window), table);
	gtk_table_set_col_spacings(GTK_TABLE(table), 1);
  	gtk_table_set_row_spacings(GTK_TABLE(table), 1);

	// Start and Reset button
	Start = gtk_button_new_with_label("Start");
	Reset = gtk_button_new_with_label("Destroy the IJVM");
	Step = gtk_button_new_with_label("Step");
	Run = gtk_button_new_with_label("Run");
	gtk_table_attach(GTK_TABLE(table), Start, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 15);
	gtk_table_attach(GTK_TABLE(table), Reset, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 15);
	gtk_table_attach(GTK_TABLE(table), Step, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 15);
	gtk_table_attach(GTK_TABLE(table), Run, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 15);
	g_signal_connect(G_OBJECT(Start), "clicked", G_CALLBACK(start), NULL);
	g_signal_connect(G_OBJECT(Reset), "clicked", G_CALLBACK(reset), NULL);
	g_signal_connect(G_OBJECT(Step), "clicked", G_CALLBACK(nextState), NULL);
	g_signal_connect(G_OBJECT(Run), "clicked", G_CALLBACK(runProgram), NULL);

	// Choose a binary file
	file_dir = gtk_entry_new();
	gtk_entry_set_text (GTK_ENTRY(file_dir), "IJVM binary file directory");
	gtk_table_attach_defaults(GTK_TABLE(table), file_dir, 4, 6, 0, 1);
	chooseButton = gtk_button_new_with_label("Choose a binary file");
	gtk_widget_set_tooltip_text(chooseButton, "Choose an IJVM binary");	
	gtk_table_attach(GTK_TABLE(table), chooseButton, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 15);
	g_signal_connect(G_OBJECT(chooseButton), "clicked", G_CALLBACK(choose), NULL);

	// Choose an input file
	file_inp = gtk_entry_new();
	gtk_entry_set_text (GTK_ENTRY(file_inp), "Input file directory");
	gtk_table_attach_defaults(GTK_TABLE(table), file_inp, 4, 6, 1, 2);
	chooseInp = gtk_button_new_with_label("Choose an input file");
	gtk_widget_set_tooltip_text(chooseInp, "Choose an input file");	
	gtk_table_attach(GTK_TABLE(table), chooseInp, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 0, 15);
	g_signal_connect(G_OBJECT(chooseInp), "clicked", G_CALLBACK(choose_inp), NULL);

	// Choose an output file
	file_out = gtk_entry_new();
	gtk_entry_set_text (GTK_ENTRY(file_out), "Output file directory");
	gtk_table_attach_defaults(GTK_TABLE(table), file_out, 4, 6, 2, 3);
	chooseOut = gtk_button_new_with_label("Choose an output file");
	gtk_widget_set_tooltip_text(chooseOut, "Choose an output file");	
	gtk_table_attach(GTK_TABLE(table), chooseOut, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 0, 15);
	g_signal_connect(G_OBJECT(chooseOut), "clicked", G_CALLBACK(choose_out), NULL);

	// Show program output
	output = gtk_text_view_new();
	outputLabel = gtk_label_new("Program output:");
	outputScroll = gtk_scrolled_window_new(NULL, NULL);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output)); 
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
	gtk_container_add(GTK_CONTAINER(outputScroll), output);
	gtk_table_attach(GTK_TABLE(table), outputLabel, 0, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(table), outputScroll, 0, 3, 3, 8, GTK_FILL, GTK_FILL, 0, 0);

	// Show program counter
	pc = gtk_text_view_new();
	pcLabel = gtk_label_new("PC:");
	pcBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pc)); 
	gtk_text_buffer_get_iter_at_offset(pcBuffer, &pcIter, 0);
	gtk_table_attach(GTK_TABLE(table), pcLabel, 3, 4, 3, 4, GTK_FILL, GTK_FILL, 15, 15);
	gtk_table_attach(GTK_TABLE(table), pc, 4, 5, 3, 4, GTK_FILL, GTK_FILL, 15, 15);


	// Show top of stack value
	ToS = gtk_text_view_new();
	ToSLabel = gtk_label_new("Top of stack:");
	ToSBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ToS)); 
	gtk_text_buffer_get_iter_at_offset(ToSBuffer, &ToSIter, 0);
	gtk_table_attach(GTK_TABLE(table), ToSLabel, 3, 4, 4, 5, GTK_FILL, GTK_FILL, 15, 15);
	gtk_table_attach(GTK_TABLE(table), ToS, 4, 5, 4, 5, GTK_FILL, GTK_FILL, 15, 15);

	// Show widgets
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);  
	gtk_main();

	return 0;
}