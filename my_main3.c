/*========== my_main.c ==========

This is the only file you need to modify in order
to get a working mdl project (for now).

my_main.c will serve as the interpreter for mdl.
When an mdl script goes through a lexer and parser,
the resulting operations will be in the array op[].

Your job is to go through each entry in op and perform
the required action from the list below:

frames: set num_frames (in misc_headers.h) for animation

basename: set name (in misc_headers.h) for animation

vary: manipluate knob values between two given frames
over a specified interval

set: set a knob to a given value

setknobs: set all knobs to a given value

push: push a new origin matrix onto the origin stack

pop: remove the top matrix on the origin stack

move/scale/rotate: create a transformation matrix
based on the provided values, then
multiply the current top of the
origins stack by it.

box/sphere/torus: create a solid object based on the
provided values. Store that in a
temporary matrix, multiply it by the
current top of the origins stack, then
call draw_polygons.

line: create a line based on the provided values. Store
that in a temporary matrix, multiply it by the
current top of the origins stack, then call draw_lines.

save: call save_extension with the provided filename

display: view the image live

jdyrlandweaver
=========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "misc_headers.h"
#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"

/*======== void first_pass()) ==========
Inputs:
Returns:

Checks the op array for any animation commands
(frames, basename, vary)

Should set num_frames and basename if the frames
or basename commands are present

If vary is found, but frames is not, the entire
program should exit.

If frames is found, but basename is not, set name
to some default value, and print out a message
with the name being used.

jdyrlandweaver
====================*/
void first_pass() {
  int i, varies;
  varies = 0;
  num_frames = 1;
  for(i=0;i<lastop;i++){
    if(op[i].opcode == FRAMES){
      num_frames = op[i].op.frames.num_frames;
    }
    if(op[i].opcode == VARY){
      varies++;
    }
    if(op[i].opcode == BASENAME){
      strcpy(name, op[i].op.basename.p->name);
      printf("Base name is %s\n", name);
    }
  }
  if(varies > 0 && num_frames ==1){
    exit(0);
  }
  if(strcmp(name, "") == 0){
    strcpy(name, "animage");
    printf("Base name is %s\n", name);
  }
  printf("Number of frames: %d\n", num_frames);
}

/*======== struct vary_node ** second_pass()) ==========
Inputs:
Returns: An array of vary_node linked lists

In order to set the knobs for animation, we need to keep
a seaprate value for each knob for each frame. We can do
this by using an array of linked lists. Each array index
will correspond to a frame (eg. knobs[0] would be the first
frame, knobs[2] would be the 3rd frame and so on).

Each index should contain a linked list of vary_nodes, each
node contains a knob name, a value, and a pointer to the
next node.

Go through the opcode array, and when you find vary, go
from knobs[0] to knobs[frames-1] and add (or modify) the
vary_node corresponding to the given knob with the
appropirate value.

05/17/12 09:29:31
jdyrlandweaver
====================*/
struct vary_node ** second_pass() {
  struct vary_node ** frames = (struct vary_node **)calloc(num_frames, sizeof(struct vary_node *));
  int i,j;
  i = 0;
  j = 0;
  double incr;
  incr = 0;
  while(i<lastop){
    if(op[i].opcode == VARY){
      for(j=0;j<num_frames;j++){
	if(!frames[j]){
	  incr = (op[i].op.vary.end_val - op[i].op.vary.start_val)/(op[i].op.vary.end_frame - op[i].op.vary.start_frame);
	  char n[128];
	  strcpy(n, op[i].op.vary.p->name);
	  printf("Knob %s\n", n);
	  struct vary_node * node = (struct vary_node *)calloc(1, sizeof(struct vary_node));
	  strcpy(node->name, n);
	  if(j<=(int)op[i].op.vary.start_frame){
	    node->value=op[i].op.vary.start_val/fabs(op[i].op.vary.end_val - op[i].op.vary.start_val);
	  }else if(j>=(int)op[i].op.vary.end_frame){
	    node->value=op[i].op.vary.end_val/fabs(op[i].op.vary.end_val - op[i].op.vary.start_val);
	  }else{
	    node->value=(op[i].op.vary.start_val + incr * (double)(j-(int)op[i].op.vary.start_frame))
	      /fabs(op[i].op.vary.end_val - op[i].op.vary.start_val);
	  }
	  if(j>=(int)op[i].op.vary.start_frame && j<=(int)op[i].op.vary.end_frame){
	    frames[j]=node;
	  }
	}else{
	  incr = (op[i].op.vary.end_val - op[i].op.vary.start_val)/(op[i].op.vary.end_frame - op[i].op.vary.start_frame);
	  char n[128];
	  strcpy(n, op[i].op.vary.p->name);
	  printf("Knob %s\n", n);
	  struct vary_node * node = (struct vary_node *)calloc(1, sizeof(struct vary_node));
	  strcpy(node->name, n);
	  if(j==(int)op[i].op.vary.start_frame){
	    node->value=op[i].op.vary.start_val/fabs(op[i].op.vary.end_val - op[i].op.vary.start_val);
	  }else if(j==(int)op[i].op.vary.end_frame){
	    node->value=op[i].op.vary.end_val/fabs(op[i].op.vary.end_val - op[i].op.vary.start_val);
	  }else if(j>(int)op[i].op.vary.start_frame && j<(int)op[i].op.vary.end_frame){
	    node->value=(op[i].op.vary.start_val + incr * (double)(j-(int)op[i].op.vary.start_frame))
	      /fabs(op[i].op.vary.end_val - op[i].op.vary.start_val);
	  }
	  struct vary_node *current = frames[j];
	  while(current->next){
	    current = current->next;
	  }
	  if(j>=(int)op[i].op.vary.start_frame && j<=(int)op[i].op.vary.end_frame){
	    current->next = node;
	  }
	}
      }
    }
    i++;
  }
  return frames;
}


/*======== void print_knobs() ==========
Inputs:
Returns:

Goes through symtab and display all the knobs and their
currnt values

jdyrlandweaver
====================*/
void print_knobs() {

  int i;

  printf( "ID\tNAME\t\tTYPE\t\tVALUE\n" );
  for ( i=0; i < lastsym; i++ ) {

    if ( symtab[i].type == SYM_VALUE ) {
      printf( "%d\t%s\t\t", i, symtab[i].name );

      printf( "SYM_VALUE\t");
      printf( "%6.2f\n", symtab[i].s.value);
    }
  }
}


/*======== void my_main() ==========
Inputs:
Returns:

This is the main engine of the interpreter, it should
handle most of the commadns in mdl.

If frames is not present in the source (and therefore
num_frames is 1, then process_knobs should be called.

If frames is present, the enitre op array must be
applied frames time. At the end of each frame iteration
save the current screen to a file named the
provided basename plus a numeric string such that the
files will be listed in order, then clear the screen and
reset any other data structures that need it.

Important note: you cannot just name your files in
regular sequence, like pic0, pic1, pic2, pic3... if that
is done, then pic1, pic10, pic11... will come before pic2
and so on. In order to keep things clear, add leading 0s
to the numeric portion of the name. If you use sprintf,
you can use "%0xd" for this purpose. It will add at most
x 0s in front of a number, if needed, so if used correctly,
and x = 4, you would get numbers like 0001, 0002, 0011,
0487

05/17/12 09:41:35
jdyrlandweaver
====================*/
void my_main( int polygons ) {

  int i, f, j;
  double step;
  double xval, yval, zval, knob_value;
  struct matrix *transform;
  struct matrix *tmp;
  struct stack *s;
  screen t;
  color g;

  struct vary_node **knobs;
  struct vary_node *vn;
  char frame_name[128];

  num_frames = 1;
  step = 5;
  f=0;

  g.red = 0;
  g.green = 255;
  g.blue = 255;

  float **z_buffer = (float **)calloc(XRES, sizeof(float *));
  for(i=0;i<XRES;i++){
      z_buffer[i] = (float *)calloc(YRES, sizeof(float));
  }
  for(i=0;i<XRES;i++){
    for(j=0;j<YRES;j++){
      z_buffer[i][j] = -1 * DBL_MAX;
    }
  }

  first_pass();
  knobs = second_pass();
  for(f = 0;f<num_frames; f++){
    struct vary_node *curr = (struct vary_node *)calloc(1, sizeof(struct vary_node));
    curr = knobs[f];
    while(curr){
      printf("%s : %f\n", curr->name, curr->value);
      curr = curr->next;
    }
  }
  for(f=0;f<num_frames;f++){
    s = new_stack();
    tmp = new_matrix(4, 4);
    transform = new_matrix(4, 4);
    if(num_frames>1){
      vn = knobs[f];
      while(vn){
        SYMTAB *symb = lookup_symbol(vn->name);
        set_value(symb, vn->value);
        vn = vn->next;
      }
    }
    //print_knobs();
    for (i=0;i<lastop;i++) {
      SYMTAB *v;
      switch (op[i].opcode) {
        case SPHERE:
          //printf("Add sphere\n");
          add_sphere( tmp,op[i].op.sphere.d[0], //cx
            op[i].op.sphere.d[1],  //cy
            op[i].op.sphere.d[2],  //cz
            op[i].op.sphere.r,
            step);
          //apply the current top origin
          matrix_mult( s->data[ s->top ], tmp );
          draw_polygons( tmp, t, g, z_buffer );
          tmp->lastcol = 0;
          break;

        case TORUS:
          //printf("Add Torus\n");
          add_torus( tmp, op[i].op.torus.d[0], //cx
            op[i].op.torus.d[1],     //cy
            op[i].op.torus.d[2],    //cz
            op[i].op.torus.r0,
            op[i].op.torus.r1,
            step);
          matrix_mult( s->data[ s->top ], tmp );
          draw_polygons( tmp, t, g, z_buffer );
            tmp->lastcol = 0;
          break;

        case BOX:
          //printf("Add box\n");
          add_box( tmp, op[i].op.box.d0[0],
            op[i].op.box.d0[1],
            op[i].op.box.d0[2],
            op[i].op.box.d1[0],
            op[i].op.box.d1[1],
            op[i].op.box.d1[2]);
          matrix_mult( s->data[ s->top ], tmp );
          draw_polygons( tmp, t, g, z_buffer );
            tmp->lastcol = 0;
          break;

        case LINE:
          //printf("Line\n");
          add_edge( tmp, op[i].op.line.p0[0],
            op[i].op.line.p0[1],
            op[i].op.line.p0[1],
            op[i].op.line.p1[0],
            op[i].op.line.p1[1],
            op[i].op.line.p1[1]);
          draw_lines( tmp, t, g, z_buffer );
            tmp->lastcol = 0;
          break;

        case MOVE:
          //printf("Move\n");
          //get the factors
          xval = op[i].op.move.d[0];
          yval =  op[i].op.move.d[1];
          zval = op[i].op.move.d[2];
          v = op[i].op.move.p;
          if(v){
            xval = xval * v->s.value;
            yval = yval * v->s.value;
            zval = zval * v->s.value;
          }
          //printf("x: %f y: %f z: %f\n", xval, yval, zval);

          transform = make_translate( xval, yval, zval );
          //multiply by the existing origin
          matrix_mult( s->data[ s->top ], transform );
          //put the new matrix on the top
          copy_matrix( transform, s->data[ s->top ] );
          free_matrix( transform );
          break;

        case SCALE:
          //printf("Scale\n");
          xval = op[i].op.scale.d[0];
          yval = op[i].op.scale.d[1];
          zval = op[i].op.scale.d[2];

          v = op[i].op.scale.p;
          if(v){
            //printf("I'm not null Scale\n");
            xval *= v->s.value;
            yval *= v->s.value;
            zval *= v->s.value;
          }

          transform = make_scale( xval, yval, zval );
          matrix_mult( s->data[ s->top ], transform );
          //put the new matrix on the top
          copy_matrix( transform, s->data[ s->top ] );
          free_matrix( transform );
          break;

        case ROTATE:
          //printf("Rotate\n");
          xval = op[i].op.rotate.degrees * ( M_PI / 180 );

          v = op[i].op.rotate.p;
          if(v){
            xval *= v->s.value;
          }
          //get the axis
          if ( op[i].op.rotate.axis == 0 )
            transform = make_rotX( xval );
          else if ( op[i].op.rotate.axis == 1 )
            transform = make_rotY( xval );
          else if ( op[i].op.rotate.axis == 2 )
            transform = make_rotZ( xval );

          matrix_mult( s->data[ s->top ], transform );
          //put the new matrix on the top
          copy_matrix( transform, s->data[ s->top ] );
          free_matrix( transform );
          break;

        case PUSH:
          //printf("Push\n");
          push( s );
          break;
        case POP:
          //printf("Pop\n");
          pop( s );
          break;
        case SAVE:
          //printf("Save\n");
          save_extension( t, op[i].op.save.p->name );
          break;
        case DISPLAY:
          //printf("Display\n");
          display( t );
          break;
      }
    }
    if(num_frames>1){
      sprintf (frame_name, "%s%03d.png", name, f);
      save_extension(t, frame_name);
    }
    clear_screen(t);
    free_stack(s);
    free_matrix(tmp);
  }

  //free_stack( s );
  //free_matrix( tmp );
  //free_matrix( transform );
}
