/**************************************************************/
/* CS/COE 1541				 			
   just compile with gcc -o pipeline pipeline.c			
   and execute using							
   ./pipeline  /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr	0  
***************************************************************/

#include <stdio.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h" 

#define BP_ENTRIES 128     // size of branch predictor table

short bp_table[BP_ENTRIES]; //1 bit branch predictor table

//get from the btb table
short get_value_from_bpt(unsigned int address){
	unsigned int mask = 127; //how to bit mask????
	unsigned int index = (address >> 4) & mask;

	return bp_table[index];
}


//send to the btb table
void set_value_to_bpt(unsigned int address, int taken){
	unsigned int mask = 127; //how to bit mask????
	unsigned int index = (address >> 4) & mask;

	bp_table[index] = taken; //do we need the ? : thing??

}


print_finished_instr(struct trace_item* instr, int cycle_number){
	switch(instr->type) {
        case ti_NOP:
          printf("[cycle %d] NOP:\n",cycle_number) ;
          break;
        case ti_RTYPE:
          printf("[cycle %d] RTYPE:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", instr->PC, instr->sReg_a, instr->sReg_b, instr->dReg);
          break;
        case ti_ITYPE:
          printf("[cycle %d] ITYPE:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", instr->PC, instr->sReg_a, instr->dReg, instr->Addr);
          break;
        case ti_LOAD:
          printf("[cycle %d] LOAD:",cycle_number) ;      
          printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", instr->PC, instr->sReg_a, instr->dReg, instr->Addr);
          break;
        case ti_STORE:
          printf("[cycle %d] STORE:",cycle_number) ;      
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", instr->PC, instr->sReg_a, instr->sReg_b, instr->Addr);
          break;
        case ti_BRANCH:
          printf("[cycle %d] BRANCH:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", instr->PC, instr->sReg_a, instr->sReg_b, instr->Addr);
          break;
        case ti_JTYPE:
          printf("[cycle %d] JTYPE:",cycle_number) ;
          printf(" (PC: %x)(addr: %x)\n", instr->PC,instr->Addr);
          break;
        case ti_SPECIAL:
          printf("[cycle %d] SPECIAL:\n",cycle_number) ;      	
          break;
        case ti_JRTYPE:
          printf("[cycle %d] JRTYPE:",cycle_number) ;
          printf(" (PC: %x) (sReg_a: %d)(addr: %x)\n", instr->PC, instr->dReg, instr->Addr);
          break;
      }
}

//queue that holds the instructions that were removed from the pipeline due to a jump or a branch
typedef struct queue_entry{
	struct trace_item entry;
	struct queue_entry* next;
	struct queue_entry* prev;
} queue_entry;
queue_entry* queue_start = 0;
queue_entry* queue_end = 0;
int queue_size = 0;

//add instruction to the queue
void add_queue_instr(struct trace_item* instr){
	queue_entry* new_entry = (queue_entry*) malloc(sizeof(queue_entry));
	new_entry->entry = *instr;
	new_entry->next = queue_start;
	new_entry->prev = 0;
	queue_entry* old_start = queue_start;
	queue_start = new_entry;
	//sets the start and end to the same thing when there is no queue left
	if(queue_size == 0){
		queue_end = queue_start;
	}else{
		old_start->prev = queue_start;
	}
	queue_size += 1;
}

//remove from the queue
int remove_queue_instr(struct trace_item* instr){
	if(queue_end == NULL){
		return 0;
	}
	*instr = queue_end->entry;
	queue_entry* new_end = queue_end->prev;
	free(queue_end);
	queue_end = new_end;
	queue_size -= 1;
	return 1;
}


//sets an instruction to a no-op
void set_instr_to_noop(struct trace_item* instruction){
	//instruction = malloc(sizeof(struct trace_item));
	//memset(instruction, 0, sizeof(struct trace_item));
	instruction->type = ti_NOP;
}

int main(int argc, char **argv)
{
  
  int branch_prediction_method = 0;
  char *trace_file_name;
  int trace_view_on = 0;
  unsigned int cycle_number = 0;
  
  unsigned char t_type = 0;
  unsigned char t_sReg_a= 0;
  unsigned char t_sReg_b= 0;
  unsigned char t_dReg= 0;
  unsigned int t_PC = 0;
  unsigned int t_Addr = 0;

  //read the inputs
  if(argc == 2){
  	trace_file_name = argv[1];
  	trace_view_on = 0;
  	branch_prediction_method = 0;
  }else if(argc == 4){
  	trace_file_name = argv[1];
  	branch_prediction_method = atoi(argv[2]); //might need a ? : operator here. unsure though
  	trace_view_on = atoi(argv[3]);
  }else{
    fprintf(stdout, "\nUSAGE: tv <trace_file> <switch - any character> <branch_prediction - 0|1|2>\n");
    fprintf(stdout, "\n(branch_prediction) 0 - no branch prediction, 1 - one bit branch prediction, 2 - two bit branch prediction \n\n");
    fprintf(stdout, "\n(switch) to turn on or off individual item view.\n");
    exit(0);
  }
  

  //open the trace file designated
  fprintf(stdout, "\n ** opening file %s\n", trace_file_name);
  trace_fd = fopen(trace_file_name, "rb");
  if (!trace_fd) {
    fprintf(stdout, "\ntrace file %s not opened.\n\n", trace_file_name);
    exit(0);
  }

  trace_init();

  //do the trace stuff

  printf("ayyo\n");
  struct trace_item *foobar;
  foobar = malloc(sizeof(struct trace_item));
  printf("ayyyo\n");
  trace_get_item(&foobar);
  printf("ayyyyo\n");
  print_finished_instr(foobar, cycle_number);
  printf("ayyyyyo\n");	

  //pipeline instructions
  fprintf(stdout, "define the stages\n");
  struct trace_item *new_instr; //reads what will be next
  new_instr = malloc(sizeof(struct trace_item));
  struct trace_item *if1_stage;
  if1_stage = malloc(sizeof(struct trace_item));
  struct trace_item *if2_stage;
  if2_stage = malloc(sizeof(struct trace_item));
  struct trace_item *id_stage;
  id_stage = malloc(sizeof(struct trace_item));
  struct trace_item *ex_stage;
  ex_stage = malloc(sizeof(struct trace_item));
  struct trace_item *mem1_stage;
  mem1_stage = malloc(sizeof(struct trace_item));
  struct trace_item *mem2_stage;
  mem2_stage = malloc(sizeof(struct trace_item));
  struct trace_item *wb_stage;
  wb_stage = malloc(sizeof(struct trace_item));
  //initialize the stages to no-ops
  fprintf(stdout, "initialize the stages\n");
  set_instr_to_noop(new_instr);
  set_instr_to_noop(if1_stage);
  set_instr_to_noop(if2_stage);
  set_instr_to_noop(id_stage);
  set_instr_to_noop(ex_stage);
  set_instr_to_noop(mem1_stage);
  set_instr_to_noop(mem2_stage);
  set_instr_to_noop(wb_stage);
  fprintf(stdout, "end of the initialization\n");

  

  //TODO do we need branch table stuff????? if so initialize here I guess


  //loop while there are still instructions left
  int instr_left = 8;
  int pc = 0; //attempt to use PC to detect same instruction infinite loop
  while(instr_left){
  	cycle_number++; //increase the cycle number
  	int hazard = 0; //initializes a no hazard state. will change based on branch detection and stuff


  	if(trace_view_on){
  		//sends the stage and cycle number of each wb stage that has finished
  		fprintf(stdout, "\n");
  		print_finished_instr(if1_stage, cycle_number);
  		print_finished_instr(if2_stage, cycle_number);
  		print_finished_instr(id_stage, cycle_number);
  		print_finished_instr(ex_stage, cycle_number);
  		print_finished_instr(mem1_stage, cycle_number);
  		print_finished_instr(mem2_stage, cycle_number);
  		print_finished_instr(wb_stage, cycle_number);
  	}


  	//attempt to use PC to detect same instruction infinite loop
  	int tempPC = wb_stage->PC;
  	if(pc == tempPC && (wb_stage->type == ti_JRTYPE || wb_stage->type == ti_JTYPE || wb_stage->type == ti_BRANCH || wb_stage->type == ti_LOAD)){
  		fprintf(stdout, "INFINITE LOOP ERROR: %d = %d", pc, tempPC);
  		exit(0);
  	}else{
  		pc = tempPC;
  	}



    //detect structural hazards
    //dectects if wb is used
    //NOTE NOT SURE IF WE NEED TO CHECK MORE TYPES LIKE ITYPE AND SPECIAL
    if(wb_stage->type == ti_LOAD || wb_stage->type == ti_RTYPE || wb_stage->type == ti_ITYPE){
        
        //if rd at wb_stage == rs or rt at id_stage
        if(wb_stage->dReg == id_stage->sReg_a || wb_stage->dReg == id_stage->sReg_b){
            hazard = 1;
            fprintf(stdout, "\nstructural hazard detected \n");
        }
    }
    
    //detect data hazards
    if(ex_stage->type == ti_LOAD && (ex_stage->dReg == id_stage->sReg_a || ex_stage->dReg == id_stage->sReg_b)){
    	hazard = 2;
    	fprintf(stdout, "\ndata hazard detected \n");

    }
    
    
    
    //detect jump control hazards
    if(ex_stage->type == ti_JTYPE || ex_stage->type == ti_JRTYPE){
    	hazard = 3;
    	fprintf(stdout, "\njump control hazard detected \n");

    }

    //detect branch control hazards
    if(ex_stage->type == ti_BRANCH){
    	fprintf(stdout, "\nbranch control hazard detected: ");

    	if(branch_prediction_method == 0){
    		if(ex_stage->PC + 4 != id_stage->PC){
    			hazard = 3; //incorrect no prediction
    			fprintf(stdout, "incorrect default (not taken) prediction\n");
    		}
    	}else if(branch_prediction_method == 1){
    		if(ex_stage->PC + 4 == id_stage->PC){//not taken
    			if(get_value_from_bpt(ex_stage->PC) == 1){//predict taken
    				hazard = 3;
    				fprintf(stdout, "predicted taken when not taken\n");
    				set_value_to_bpt(ex_stage->PC, 0);
    			}
    		}else{
    			if(get_value_from_bpt(ex_stage->PC) == 0){//predict not taken
    				hazard = 3;
    				fprintf(stdout, "predicted not taken when taken\n");
    				set_value_to_bpt(ex_stage->PC, 1);
    			}
    		}
    	}else if(branch_prediction_method == 2){

    	}
    }



    //moved these here since they are common across all hazards
    wb_stage = mem2_stage;
  	mem2_stage = mem1_stage;
  	mem1_stage = ex_stage;


  	//hazard switching
  	switch(hazard){
  		case 0: //no hazard
  			//TODO
  			ex_stage = id_stage;
  			id_stage = if2_stage;
  			if2_stage = if1_stage;
  			if1_stage = new_instr;

  			//get next instr, if none decrement the instr_left
  			if(!trace_get_item(&new_instr)){
  				instr_left -= 1;
  				set_instr_to_noop(new_instr);
  			}

  			break;

  		case 1: //structural hazard
  			//move up ex, mem1, mem2, and wb
  			//dont change if1, if2, and id, and don't fetch a new instruction
  			set_instr_to_noop(ex_stage);
  			break;

  		case 2: //data hazard
  			set_instr_to_noop(ex_stage);
  			//do we have to implement our own forwarding??
  			//set id_stage.reg = ex_stage.reg
  			break;

  		case 3: //control hazard
  			//flush IF1, IF2, and ID
  			add_queue_instr(if1_stage);
  			add_queue_instr(if2_stage);
  			add_queue_instr(id_stage);

  			set_instr_to_noop(if1_stage);
  			set_instr_to_noop(if2_stage);
  			set_instr_to_noop(id_stage);

  			ex_stage = id_stage;
  			id_stage = if2_stage;
  			if2_stage = if1_stage;
  			if1_stage = new_instr;

  			//get next instr, if none decrement the instr_left
  			if(!trace_get_item(&new_instr)){
  				instr_left -= 1;
  				set_instr_to_noop(new_instr);
  			}

  			break;
  		default:
  			exit(1); //exit with an error. should never hit this

  	}
  	
  }

  printf(" Simulation terminates at cycle: %u \n\n", cycle_number);

  trace_uninit();

  exit(0);
}

