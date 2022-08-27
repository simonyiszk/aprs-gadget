#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main(void){

    //*** PARAMETERS

    #define F_NUM 5
    uint32_t F_CLK[F_NUM] = {16, 32, 45, 48, 64}; // AHB clock values in MHz for TIMERS

    uint16_t frequency_1 = 1200;
    uint16_t frequency_2 = 2200;

    double precision = 0.000;

    uint16_t min_samps = 16;
    uint16_t min_period = 64;

    uint16_t max_prsc = 200;
    uint16_t max_period = 512;
    uint16_t max_samps = 512;

    //***

    uint16_t s_1_low = frequency_1*(1.0-precision);
    uint16_t s_1_hi = frequency_1*(1.0+precision);

    uint16_t s_2_low = frequency_2*(1.0-precision);
    uint16_t s_2_hi = frequency_2*(1.0+precision);

	printf("Precision: %.2f %%\n", precision * 100);

    for(uint8_t f=0; f<F_NUM; f++){
		uint8_t fclk_printed = 0;
        for(uint16_t period = min_period; period < max_period; period++){
            for(uint16_t samps = min_samps; samps < max_samps; samps++){
                for(uint16_t prsc_1 = 0; prsc_1 < max_prsc; prsc_1++){
					uint32_t fpwm_1 = F_CLK[f] * 1000000 / (prsc_1 + 1) / (period + 1);
                    uint16_t signal_1_freq = fpwm_1 / samps;
                    if(signal_1_freq <= s_1_hi && s_1_low <= signal_1_freq){
                        for(uint16_t prsc_2 = 0; prsc_2 < max_prsc; prsc_2++){
							uint32_t fpwm_2 = F_CLK[f] * 1000000 / (prsc_2 + 1) / (period + 1);
                            uint16_t signal_2_freq = fpwm_2 / samps;
                            if(signal_2_freq <= s_2_hi && s_2_low <= signal_2_freq){
								if(!fclk_printed){
									printf(" F_CLK for TIMER: %3d MHz\n", F_CLK[f]);
									fclk_printed = 1;
								}
                                printf("F_SIG: %4d, %4d [Hz]\tPRSC: %5d, %5d\tPERIOD: %d\tSAMPLES: %3d\tRES: %2.3f bit\tF_PWM: %6d, %6d\n", 
                                signal_1_freq, signal_2_freq, prsc_1, prsc_2, period, samps, log2(period+1), fpwm_1, fpwm_2);
                            }
                        }
                    }
                }
            }
        }
    }

    printf(" END\nGenerate samples? [Y/n] ");
    char chr = getc(stdin);

    if(chr =='Y' || chr =='y'){
        uint16_t sample_count = 0;
        uint16_t dc = 0;

		printf("Enter counter PERIOD (or max duty cycle): ");
        scanf("%hu", &dc);
		
        printf("Enter SAMPLE count: ");
        scanf("%hu", &sample_count);

        FILE* code = fopen("sine.txt", "w");
        fprintf(code, "uint16_t sine[%d] = {", sample_count);
        
        FILE* gnuplot = popen("gnuplot -persistent", "w");
        fprintf(gnuplot, "bind Close \"exit gnuplot\"\n");
        fprintf(gnuplot, "set terminal wxt title \"PWM samples for DDS\"\n");
        fprintf(gnuplot, "set terminal wxt size 1300,600\n");
        fprintf(gnuplot, "set datafile separator ','\n");
        // fprintf(gnuplot, "set xrange [0:405]\n");
        fprintf(gnuplot, "set grid\n");
        fprintf(gnuplot, "set ylabel \"Amplitude\"\n");
        fprintf(gnuplot, "set xlabel \"Sample\"\n");
        fprintf(gnuplot, "plot '-' using 1 with lines lw 1.5 lt 6 title 'sine'\n");
        
		double range = dc / 2.0;

        for(int i=0; i<sample_count; i++){
			fprintf(code, "0x%04X%s", (uint16_t) (range + (range * sin(2*M_PI*i/sample_count))), (i!=sample_count-1 ? "," : ""));
			fprintf(gnuplot, "%u\n", (uint16_t) (range + (range * sin(2*M_PI*i/sample_count))));
        }

        fprintf(gnuplot, "e\n");
        fprintf(code, "};\n");
        fclose(code);
        fclose(gnuplot);
    }

    return 0;
}