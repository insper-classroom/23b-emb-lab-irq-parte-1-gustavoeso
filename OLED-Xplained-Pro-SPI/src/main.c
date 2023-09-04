/************************************************************************/
/* includes                                                             */
/************************************************************************/
#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

//configuracoes do LED 1 (PA0)
#define OLED_LED_1              PIOA
#define OLED_LED_1_ID           ID_PIOA
#define OLED_LED_1_IDX          0
#define OLED_LED_1_IDX_MASK     (1u << OLED_LED_1_IDX)

//configuracao do LED 2 (PC30)
#define OLED_LED_2         PIOC
#define OLED_LED_2_ID      ID_PIOC
#define OLED_LED_2_IDX     30
#define OLED_LED_2_IDX_MASK    (1u << OLED_LED_2_IDX)

//configuracao do LED 3 (PB2)
#define OLED_LED_3             PIOB
#define OLED_LED_3_ID          ID_PIOB
#define OLED_LED_3_IDX         2
#define OLED_LED_3_IDX_MASK    (1u << OLED_LED_3_IDX)

//configuracao do BUT 1 (PD28)
#define OLED_BUT1             PIOD
#define OLED_BUT1_ID          ID_PIOD
#define OLED_BUT1_IDX         28
#define OLED_BUT1_IDX_MASK    (1u << OLED_BUT1_IDX)

//configuracao do BUT 2 (PC31)
#define OLED_BUT2             PIOC
#define OLED_BUT2_ID          ID_PIOC
#define OLED_BUT2_IDX         31
#define OLED_BUT2_IDX_MASK    (1u << OLED_BUT2_IDX)

//configuracao do BUT 3 (PA19)
#define OLED_BUT3             PIOA
#define OLED_BUT3_ID          ID_PIOA
#define OLED_BUT3_IDX         19
#define OLED_BUT3_IDX_MASK    (1u << OLED_BUT3_IDX)

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

volatile char but_flag1;
volatile char but_flag2 = 0;
volatile char but_flag3 = 0;
volatile char time_stop = 0;
volatile char time_start = 0;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/
void io_init(void);
void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
void pisca_led2(int n, int t);
int pisca_led(int n, int t, int i);

/************************************************************************/
/* handler / callbacks                                                  */
/************************************************************************/
void but1_callback(void)
{
	if (pio_get(OLED_BUT1, PIO_INPUT, OLED_BUT1_IDX_MASK)) {
		// PINO == 1 --> Borda de subida
		time_stop = 1;
		time_start = 0;
		but_flag1 = 1;
		} else {
		// PINO == 0 --> Borda de descida
		but_flag1 = 0;
		time_start = 1;
		time_stop = 0;
		}
	but_flag2 = 0;
}

void but2_callback(void)
{
	but_flag2 = 1;
}

void but3_callback(void)
{
	but_flag3 = 1;
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/
// pisca led N vez no periodo T
int pisca_led(int n, int t, int i){
	for (;i<n;i++){
		pio_clear(OLED_LED_1, OLED_LED_1_IDX_MASK);
		delay_ms(t);
		pio_set(OLED_LED_1, OLED_LED_1_IDX_MASK);
		delay_ms(t);
		
		if(but_flag2){
			return i+1;
		}
	}
	return i;
}

// Inicializa botao SW0 do kit com interrupcao
void io_init(void)
{
	// Configura Oled 1
	pmc_enable_periph_clk(OLED_LED_1_ID);
	pio_configure(OLED_LED_1, PIO_OUTPUT_0, OLED_LED_1_IDX_MASK, PIO_DEFAULT);

	// Inicializa clock do periferico PIO responsavel pelos botoes
	pmc_enable_periph_clk(OLED_BUT1_ID);
	pmc_enable_periph_clk(OLED_BUT2_ID);
	pmc_enable_periph_clk(OLED_BUT3_ID);
	

	// Configura PIO para lidar com o pino do botao 1 como entrada
	// com pull-up
	pio_configure(OLED_BUT1, PIO_INPUT, OLED_BUT1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(OLED_BUT1, OLED_BUT1_IDX_MASK, 60);
	
	// Configura PIO para lidar com o pino do botao 2 como entrada
	// com pull-up
	pio_configure(OLED_BUT2, PIO_INPUT, OLED_BUT2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(OLED_BUT2, OLED_BUT2_IDX_MASK, 60);
	
	// Configura PIO para lidar com o pino do botao 3 como entrada
	// com pull-up
	pio_configure(OLED_BUT3, PIO_INPUT, OLED_BUT3_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(OLED_BUT3, OLED_BUT3_IDX_MASK, 60);

	// Configura interrupcao no pino referente ao botao 1 e associa
	// funcao de callback caso uma interrupcao for gerada
	// a funcao de callback a: but_callback()
	pio_handler_set(OLED_BUT1,
	OLED_BUT1_ID,
	OLED_BUT1_IDX_MASK,
	PIO_IT_EDGE,
	but1_callback);
	
	// Configura interrupcao no pino referente ao botao 2 e associa
	// funcao de callback caso uma interrupcao for gerada
	// a funcao de callback a: but_callback()
	pio_handler_set(OLED_BUT2,
	OLED_BUT2_ID,
	OLED_BUT2_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but2_callback);

	// Configura interrupcao no pino referente ao botao 3 e associa
	// funcao de callback caso uma interrupcao for gerada
	// a funcao de callback a: but_callback()	
	pio_handler_set(OLED_BUT3,
	OLED_BUT3_ID,
	OLED_BUT3_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but3_callback);

	// Ativa interrupcao e limpa primeira IRQ gerada na ativacao botao 1
	pio_enable_interrupt(OLED_BUT1, OLED_BUT1_IDX_MASK);
	pio_get_interrupt_status(OLED_BUT1);
	
	// Ativa interrupcao e limpa primeira IRQ gerada na ativacao botao 2
	pio_enable_interrupt(OLED_BUT2, OLED_BUT2_IDX_MASK);
	pio_get_interrupt_status(OLED_BUT2);
	
	// Ativa interrupcao e limpa primeira IRQ gerada na ativacao botao 3
	pio_enable_interrupt(OLED_BUT3, OLED_BUT3_IDX_MASK);
	pio_get_interrupt_status(OLED_BUT3);
	
	// Configura NVIC para receber interrupcoes do PIO do botao 1
	// com prioridade 4 (quanto mais proximo de 0 maior)
	NVIC_EnableIRQ(OLED_BUT1_ID);
	NVIC_SetPriority(OLED_BUT1_ID, 4); // Prioridade 4
	
	// Configura NVIC para receber interrupcoes do PIO do botao 2
	// com prioridade 4 (quanto mais proximo de 0 maior)
	NVIC_EnableIRQ(OLED_BUT2_ID);
	NVIC_SetPriority(OLED_BUT2_ID, 4); // Prioridade 4
	
	// Configura NVIC para receber interrupcoes do PIO do botao 3
	// com prioridade 4 (quanto mais proximo de 0 maior)
	NVIC_EnableIRQ(OLED_BUT3_ID);
	NVIC_SetPriority(OLED_BUT3_ID, 4); // Prioridade 4
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();
	int delay = 500;
	char str[128];
	int tempo = 0;
	int i = 0;
	
	// Inicializa clock
	sysclk_init();

	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// configura botao com interrupcao
	io_init();

	// Init OLED
	gfx_mono_ssd1306_init();
	
	
	gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
	sprintf(str, "%lf", (double)500/delay);
	gfx_mono_draw_string(str, 50, 16, &sysfont);
	

	/* Insert application code here, after the board has been initialized. */
	while(1) {
		
		if(but_flag2){
			while(but_flag2){}
		}
		
		while(time_start){
			delay_ms(100);
			tempo += 100;
		}
		
		
		if(time_stop && but_flag1){
			time_stop = 0;
			if(tempo < 1000 && delay > 100){
				delay -= 100;
			} else {
				delay += 100;
			}
		}
		
		tempo = 0;
		sprintf(str, "%lf", (double)500/delay);
		gfx_mono_draw_string(str, 50, 16, &sysfont);
		
		if(but_flag1){
			i = pisca_led(5, delay, i);
			if(i >= 5){
				i = 0;
			}
			but_flag1 = 0;
		}

		if(but_flag3){
			delay += 100;
			sprintf(str, "%lf", (double)500/delay);
			gfx_mono_draw_string(str, 50, 16, &sysfont);
			i = pisca_led(5, delay, i);
			if(i >= 5){
				i = 0;
			}
			but_flag3 = 0;	
		}
		
		
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}
