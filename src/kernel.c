#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

static const size_t VGA_WIDTH = 80; // vga buffer width - width of the screen
static const size_t VGA_HEIGHT = 25; // vga buffer height - height of the screen

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) 
{
	terminal_row = VGA_HEIGHT-2;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK); // sets the text and background colours to light gray and black.
	terminal_buffer = (uint16_t*) 0xB8000; // 

	for (size_t y = 0; y < VGA_HEIGHT; y++) {		// for every row in the vga buffer
		for (size_t x = 0; x < VGA_WIDTH; x++) { 				// loop through each column
			const size_t index = y * VGA_WIDTH + x; 					// calculate the index of the current space in the vga buffer 
			terminal_buffer[index] = vga_entry(' ', terminal_color); 		// sets the current character in the vga buffer to " "
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_scroll() {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {		// for every row in the vga buffer
		for (size_t x = 0; x < VGA_WIDTH; x++) {		// loop through each column

			const size_t index = y * VGA_WIDTH + x;  // calculate the current index in the vga buffer
			const size_t indexup = (y-1) * VGA_WIDTH + x;	// increment the y value by -1 to calculate the index of the index one row up

			char c = terminal_buffer[index];	// saves the character in the current vga buffer index		
			terminal_buffer[indexup] = vga_entry(c, terminal_color);	// and sets it to the one above it
		}
	}
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x; 
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) 
{
	if (c != '\n') terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		terminal_scroll();
	}
	if (c == '\n') {
		terminal_scroll();
		terminal_column = 0;
	}
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void kernel_welcome() {
	size_t old = terminal_row;
	terminal_row = 1;

	terminal_writestring("Welcome to ArcOS :-)");
	terminal_row = old;
	terminal_column = 0;
}

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
	kernel_welcome();

	//terminal_writestring("hi");
}
