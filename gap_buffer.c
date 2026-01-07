#include "gap_buffer.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

GapBuffer *gb_create(size_t GAP_SIZE) {
    GapBuffer *gb = (GapBuffer *)malloc(sizeof(GapBuffer));
    if(!gb) return NULL;

    gb->point = NULL;
    gb->buffer = NULL;
    gb->bufend = NULL;
    gb->gapstart = NULL;
    gb->gapend = NULL;
    gb->GAP_SIZE = GAP_SIZE;
}

int gb_init_buffer(GapBuffer *gb,size_t size) {
    if(gb->buffer) {
        free(gb->buffer);
    }

    gb->buffer = malloc(size);
    if(!gb->buffer) {
        return 0;
    }
    gb->point = gb->buffer;
    gb->gapstart = gb->buffer; // initial gapsize = full buffer
    gb->gapend = gb->buffer + size;
    gb->bufend = gb->gapend;

    return 1;
} 

int gb_copy_bytes(GapBuffer *gb,char *dest,char *src,size_t len) {
    if((dest == src) || len == 0) {
        return 1;
    }
    //check to make sure that we dont go beyond the buffer
    if(src > dest) {
        if((src + len) >= gb->bufend) {
            return 0;
        }
        for(;len>0;len--) {
            *(dest++) = *(src++);
        }
    }
    else {
        //to prevent overwriting the charecters we need to move forward and copy
        src+=len;
        dest+=len;

        for(;len>0;len--) {
            //predecrement because we start one byte beyond where we want after adding the length.
            *(--dest) = *(--src);
        }

    }
    return 1;
}

void gb_move_gap_to_point(GapBuffer *gb) {
    if(gb->point == gb->gapstart) {
        return;
    }
    if(gb->point == gb->gapend) {
        gb->point = gb->gapstart;
        return;
    }
    if(gb->point < gb->gapstart) {
        //move gap towards left 
        //move the point over by gapsize
        gb_copy_bytes(gb,(gb->point+(gb->gapend - gb->gapstart)),gb->point,gb->gapstart - gb->point); //dest,src,len
        gb->gapend-=(gb->gapstart - gb->point);
        gb->gapstart = gb->point;
    }

    else {
        //Since the point will be after the gap ,
        //find distance gapend and the point
        //and thats how much we move from gapend to gapstart
        //we are moving the bytes after the gapend till the point to the start aste
        gb_copy_bytes(gb,gb->gapstart,gb->gapend,(gb->point - gb->gapend));
        gb->gapstart+=(gb->point-gb->gapend);
        gb->gapend = gb->point;
        gb->point = gb->gapstart;
    }
}

void gb_expand_buffer(GapBuffer *gb,size_t size) {
    //check to see if we actually need to increase the size of the buffer
    //since the buffer size doesn't include gaps
    if((gb->bufend - gb->buffer)+size > gb_buffer_size(gb)) {
        char *origBuffer = gb->buffer;
        int newBufferSize = (gb->bufend - gb->buffer)+size+gb->GAP_SIZE;
        gb->buffer = (char *)realloc(gb->buffer,newBufferSize);

        gb->point += (gb->buffer - origBuffer);
        gb->bufend += (gb->buffer - origBuffer);
        gb->gapstart += (gb->buffer - origBuffer);
        gb->gapend += (gb->buffer - origBuffer);
        
    }
}


void gb_expand_gap(GapBuffer *gb,size_t size) {
    if(size > gb_size_of_gap(gb)) {
        size+=gb->GAP_SIZE;
        gb_expand_buffer(gb,size);
        //move the text after the gap to the right
        gb_copy_bytes(gb,(gb->gapend+size),gb->gapend,(gb->bufend-gb->gapend));

        gb->gapend+=size;
        gb->bufend+=size;
    }

}

size_t gb_size_of_gap(GapBuffer *gb){
    return (size_t)(gb->gapend - gb->gapstart);
}

size_t gb_buffer_size(GapBuffer *gb) {
    return (size_t)((gb->bufend - gb->buffer) - (gb->gapend - gb->gapstart));
}

size_t gb_point_offset(GapBuffer *gb) {
    if(gb->point >= gb->gapend)
        return (size_t)((gb->point - gb->buffer) - (gb->gapend - gb->gapstart));
    else 
        return (size_t)(gb->point - gb->buffer);
}

void gb_set_point(GapBuffer *gb,size_t offset){
    gb->point = gb->buffer + offset;

    //if the position lands on a gap,we should skip it (add the size)
    if(gb->point > gb->gapstart) {
        gb->point += (gb->gapend - gb->gapstart);
    }
}

void gb_free(GapBuffer *gb){
    if(!gb) return;
    free(gb->buffer);
    free(gb);
}

char gb_get_char(GapBuffer *gb) {
    if(gb->point == gb->gapstart) {
        gb->point = gb->gapend;
    }

    return *gb->point;
}

void gb_insert_char(GapBuffer *gb,char ch) {
    if(gb->point != gb->gapstart) {
        gb_move_gap_to_point(gb);
    }
    if(gb->gapstart == gb->gapend) {
        gb_expand_gap(gb,1);
    }
    *(gb->gapstart) = ch;
}

int main(){
    return 0;
}
