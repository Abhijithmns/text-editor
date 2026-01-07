#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stdio.h>
#include <stddef.h>

typedef struct {
    char *point;   // location pointer into buffer
    char *buffer;  // start of the text buffer
    char *bufend;  //first location outside the buffer
    char *gapstart; // start of gap
    char *gapend; //first location after the end of gap
    size_t GAP_SIZE;
}GapBuffer;

GapBuffer *gb_create(size_t GAP_SIZE);
GapBuffer *gb_create_from_file(FILE *file,size_t GAP_SIZE);
GapBuffer *gb_copy(const GapBuffer *src);
void gb_free(GapBuffer *gb); //done

//helpers

int gb_init_buffer(GapBuffer *gb,size_t size); //marked
int gb_copy_bytes(GapBuffer *gb,char *dest,char *src,size_t len);
void gb_expand_buffer(GapBuffer *gb,size_t size);
void gb_expand_gap(GapBuffer *gb,size_t size);

//opearations
void gb_move_gap_to_point(GapBuffer *gb);
void gb_set_point(GapBuffer *gb,size_t offset); //done
size_t gb_size_of_gap(GapBuffer *gb); //done
size_t gb_point_offset(GapBuffer *gb); //done
size_t gb_buffer_size(GapBuffer *gb); //done


char gb_get_char(GapBuffer *gb); //marked
char gb_prev_char(GapBuffer *gb);
char gb_next_char(GapBuffer *gb);
void gb_replace_char(GapBuffer *gb, char ch); //marked
void gb_put_char(GapBuffer *gb, char ch); //marked
void gb_insert_char(GapBuffer *gb, char ch); //marked
void gb_delete_chars(GapBuffer *gb, size_t count); //marked
void gb_insert_string(GapBuffer *gb, const char *str, size_t len);

//final functions
void gb_print(const GapBuffer *gb); //marked
int  gb_save_to_file(GapBuffer *gb, FILE *file, size_t bytes);


#endif
