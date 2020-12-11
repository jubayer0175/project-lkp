#include <stdint.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#define SIZE_OF_STAT 1000
#define BOUND_OF_LOOP 500



#include<mpt/pkey.h>
#include <mpt/mpt.h>



void static inline Filltimes(uint64_t **times) {
	int i, j;
	unsigned long long start, end;
	unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
	volatile int variable = 0;

	asm volatile ("CPUID\n\t"
	 "RDTSC\n\t"
	 "mov %%edx, %0\n\t"
	 "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
	"%rax", "%rbx", "%rcx", "%rdx");
	
	asm volatile("RDTSCP\n\t"
	 "mov %%edx, %0\n\t"
	 "mov %%eax, %1\n\t"
	"CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
	"%rbx", "%rcx", "%rdx");
	
	asm volatile ("CPUID\n\t"
	 "RDTSC\n\t"
	 "mov %%edx, %0\n\t"
	 "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
	"%rax", "%rbx", "%rcx", "%rdx");
	
	asm volatile("RDTSCP\n\t"
	 "mov %%edx, %0\n\t"
	 "mov %%eax, %1\n\t"
	"CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
	"%rbx", "%rcx", "%rdx");
	for (j=0; j<BOUND_OF_LOOP; j++) {
	    for (i =0; i<SIZE_OF_STAT; i++) {
	        variable = 0;
		
	        asm volatile ("CPUID\n\t"
	                    "RDTSC\n\t"
	                    "mov %%edx, %0\n\t"
	                    "mov %%eax, %1\n\t": "=r" (cycles_high), "=r"
	                    (cycles_low):: "%rax", "%rbx", "%rcx", "%rdx");
	/***********************************/
	/* Will call mpk inplemented function and not MPK version of the code*/
	/***********************************/

    asm volatile("RDTSCP\n\t"
	 "mov %%edx, %0\n\t"
	 "mov %%eax, %1\n\t"
	 "CPUID\n\t": "=r" (cycles_high1), "=r"
	(cycles_low1):: "%rax", "%rbx", "%rcx", "%rdx");
	//raw_local_irq_restore(flags);
	//preempt_enable();


	start = ( ((uint64_t)cycles_high << 32) | cycles_low );
	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );
	if ( (end - start) < 0) {
	printf("\n\n Time is not right\n loop(%d) stat(%d) start = %llu, end = %llu, variable = %u\n", j, i, start, end, variable);
		times[j][i] = 0;
			}
		else
	{
		times[j][i] = end - start;
	 }
	  }
	}
	   return;
 }





uint64_t var_calc(uint64_t *inputs, int size)
{
int i;
uint64_t acc = 0, previous = 0, temp_var = 0;
for (i=0; i< size; i++) {
	if (acc < previous) goto overflow;
	previous = acc;
	acc += inputs[i];
	}
    acc = acc * acc;
    if (acc < previous) goto overflow;
            previous = 0;
    for (i=0; i< size; i++){
        if (temp_var < previous) goto overflow;
            previous = temp_var;
            temp_var+= (inputs[i]*inputs[i]);
            }
    temp_var = temp_var * size;
    if (temp_var < previous) goto overflow;
    temp_var =(temp_var - acc)/(((uint64_t)(size))*((uint64_t)(size)));
    return (temp_var); 
        overflow: printf("\n\n>>>>>>>>>>>>>> CRITICAL OVERFLOW ERROR IN var_calc!!!!!!\n\n");
    return -EINVAL;
    }






int main(void)
{
int i = 0, j = 0, spurious = 0, k =0;
uint64_t **times;
uint64_t *variances;

uint64_t *min_values;
uint64_t max_dev = 0, min_time = 0, max_time = 0, prev_min =0, tot_var=0,
	max_dev_all=0, var_of_vars=0, var_of_mins=0;

    printf("Start measuring...\n");
    times = malloc(BOUND_OF_LOOP*sizeof(uint64_t*));
if (!times) {
printf("unable to allocate memory for times\n");
return 0;
}

for (j=0; j<BOUND_OF_LOOP; j++) {
        times[j] = malloc(SIZE_OF_STAT*sizeof(uint64_t));
        if (!times[j]) {
            printf("unable to allocate memory for times[%d]\n", j);
            for (k=0; k<j; k++)
                free(times[k]);
            return 0;
            
            }
        }

 variances = malloc(BOUND_OF_LOOP*sizeof(uint64_t));
    if (!variances) {
printf("unable to allocate memory for variances\n");
return 0;
}

        min_values = malloc(BOUND_OF_LOOP*sizeof(uint64_t));
        if (!min_values) {
         printf("unable to allocate memory for min_values\n");
        return 0;
 }


Filltimes(times);/* This is my utimate bro*/

for (j=0; j<BOUND_OF_LOOP; j++) {

	max_dev = 0;
	min_time = 0;
	max_time = 0;

for (i =0; i<SIZE_OF_STAT; i++) {
        if ((min_time == 0)||(min_time > times[j][i]))
	        min_time = times[j][i];
        if (max_time < times[j][i])
	        max_time = times[j][i];
        }

	max_dev = max_time - min_time;
 	min_values[j] = min_time;

if ((prev_min != 0) && (prev_min > min_time))
	spurious++;
if (max_dev > max_dev_all)
 	    max_dev_all = max_dev;

	variances[j] = var_calc(times[j], SIZE_OF_STAT);
	tot_var += variances[j];

        printf("variance(cycles): %lu; max_deviation: %lu ;min time: %lu\n", variances[j], max_dev, min_time);
 	prev_min = min_time;
}

var_of_vars = var_calc(variances, BOUND_OF_LOOP);
var_of_mins = var_calc(min_values, BOUND_OF_LOOP);
	printf("\n total number of spurious min values = %d", spurious);
	printf("\n total variance = %lu", (tot_var/BOUND_OF_LOOP));
	printf("\n absolute max deviation = %lu", max_dev_all);
	printf("\n variance of variances = %lu", var_of_vars);
	printf("\n variance of minimum values = %lu", var_of_mins);
for (j=0; j<BOUND_OF_LOOP; j++) {
	    free(times[j]);
}
	    free(times);
	    free(variances);
	    free(min_values);
	return 0;
}


