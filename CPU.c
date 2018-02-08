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

print_finished_instr(struct trace_item* instr, int cycle_number){
	switch(instr->type) {
        case ti_NOP:
          printf("[cycle %d] NOP\n:",cycle_number) ;
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

//sets an instruction to a no-op
void set_intsr_to_noop(struct trace_item* instruction){
	memset(instruction, 0, sizeof(struct trace_item));
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
  	trace_view_on = atoi(argv[2]);
  	branch_prediction_method = atoi(argv[3]); //might need a ? : operator here. unsure though
  }else{
    fprintf(stdout, "\nUSAGE: tv <trace_file> <switch - any character> <branch_prediction - 0|1|2>\n");
    fprintf(stdout, "\n(switch) to turn on or off individual item view.\n");
    fprintf(stdout, "\n(branch_prediction) 0 - no branch prediction, 1 - one bit branch prediction, 2 - two bit branch prediction \n\n");
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

  //pipeline instructions
  struct trace_item new_instr; //reads what will be next
  struct trace_item if1_stage;
  struct trace_item if2_stage;
  struct trace_item id_stage;
  struct trace_item ex_stage;
  struct trace_item mem1_stage;
  struct trace_item mem2_stage;
  struct trace_item wb_stage;
  //initialize the stages to no-ops
  set_intsr_to_noop(&new_instr);
  set_intsr_to_noop(&if1_stage);
  set_intsr_to_noop(&if2_stage);
  set_intsr_to_noop(&id_stage);
  set_intsr_to_noop(&ex_stage);
  set_intsr_to_noop(&mem1_stage);
  set_intsr_to_noop(&mem2_stage);
  set_intsr_to_noop(&wb_stage);

  //TODO do we need branch table stuff????? if so initialize here I guess


  //loop while there are still instructions left
  int instr_left = 7;
  while(instr_left){
  	cycle_number++; //increase the cycle number

  	if(trace_view_on){
  		//sends the stage and cycle number of each wb stage that has finished
  		print_finished_instr(&wb_stage, cycle_number);
  	}

  	int hazard = 0; //initializes a no hazard state. will change based on branch detection and stuff

  	//TODO: some branch detection shit. lots of loops and conditions




  	//hazard switching
  	switch(hazard){
  		case 0: //no hazard
  			//TODO
  			break;

  		case 1: //structural hazard
  			//TODO
  			break;

  		case 2: //data hazard
  			//TODO
  			break;

  		case 3: //control hazard
  			//flush IF1, IF2, and ID
  			set_intsr_to_noop(&if1_stage);
  			set_intsr_to_noop(&if2_stage);
  			set_intsr_to_noop(&id_stage);
  			break;

  	}






  }

  printf(" Simulation terminates at cycle: %u \n\n", cycle_number);

  trace_uninit();

  exit(0);
}

