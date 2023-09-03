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

//#define LED_PIO           PIOC                 // periferico que controla o LED
// # (1)
//#define LED_PIO_ID        ID_PIOC                  // ID do periférico PIOC (controla LED)
//#define LED_PIO_IDX       8                    // ID do LED no PIO
//#define LED_PIO_IDX_MASK  (1u << LED_PIO_IDX)   // Mascara para CONTROLARMOS o LED

// Configuracoes do botao
//#define BUT_PIO           PIOA
//#define BUT_PIO_ID        ID_PIOA
//#define BUT_PIO_IDX       11
//#define BUT_PIO_IDX_MASK (1u << BUT_PIO_IDX) // esse já está pronto.

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
volatile char but_flag2;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/
void io_init(void);
void pisca_led(int n, int t);

/************************************************************************/
/* handler / callbacks                                                  */
/************************************************************************/
void but_callback1(void)
{
	but_flag1 = 1;
}

void but_callback2(void)
{
	but_flag2 = 1;
}

/************************************************************************/
/* funções                                                              */
/************************************************************************/
// pisca led N vez no periodo T
void pisca_led(int n, int t){
	for (int i=0;i<n;i++){
		pio_clear(OLED_LED_1, OLED_LED_1_IDX_MASK);
		delay_ms(t);
		pio_set(OLED_LED_1, OLED_LED_1_IDX_MASK);
		delay_ms(t);
	}
}

// Inicializa botao SW0 do kit com interrupcao
void io_init(void)
{

	// Configura Oled 1
	pmc_enable_periph_clk(OLED_LED_1_ID);
	pio_configure(OLED_LED_1, PIO_OUTPUT_0, OLED_LED_1_IDX_MASK, PIO_DEFAULT);

	// Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(OLED_BUT1_ID);

	// Configura PIO para lidar com o pino do botão 1 como entrada
	// com pull-up
	pio_configure(OLED_BUT1, PIO_INPUT, OLED_BUT1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(OLED_BUT1, OLED_BUT1_IDX_MASK, 60);
	
	// Configura PIO para lidar com o pino do botão 2 como entrada
	// com pull-up
	pio_configure(OLED_BUT2, PIO_INPUT, OLED_BUT2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(OLED_BUT2, OLED_BUT2_IDX_MASK, 60);

	// Configura interrupção no pino referente ao botao 1 e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(OLED_BUT1,
	OLED_BUT1_ID,
	OLED_BUT1_IDX_MASK,
	PIO_IT_RISE_EDGE,
	but_callback1);
	
	// Configura interrupção no pino referente ao botao 2 e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(OLED_BUT2,
	OLED_BUT2_ID,
	OLED_BUT2_IDX_MASK,
	PIO_IT_RISE_EDGE,
	but_callback2);

	// Ativa interrupção e limpa primeira IRQ gerada na ativacao botao 1
	pio_enable_interrupt(OLED_BUT1, OLED_BUT1_IDX_MASK);
	pio_get_interrupt_status(OLED_BUT1);
	
	// Ativa interrupção e limpa primeira IRQ gerada na ativacao botao 2
	pio_enable_interrupt(OLED_BUT2, OLED_BUT2_IDX_MASK);
	pio_get_interrupt_status(OLED_BUT2);
	
	// Configura NVIC para receber interrupcoes do PIO do botao 1
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(OLED_BUT1_ID);
	NVIC_SetPriority(OLED_BUT1_ID, 4); // Prioridade 4
	
	// Configura NVIC para receber interrupcoes do PIO do botao 2
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(OLED_BUT2_ID);
	NVIC_SetPriority(OLED_BUT2_ID, 4); // Prioridade 4
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();
	int frequencia_default = 500;
	
	// Inicializa clock
	sysclk_init();

	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// configura botao com interrupcao
	io_init();

  // Init OLED
	gfx_mono_ssd1306_init();
  
  
	gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
	gfx_mono_draw_string("mundo", 50,16, &sysfont);
  
  

  /* Insert application code here, after the board has been initialized. */
	while(1) {
			
			if(but_flag1){
				pisca_led(5, frequencia_default -= 100);
				but_flag1 = 0;
			}
			if(but_flag2){
				pisca_led(5, frequencia_default += 100);
				but_flag2 = 0;
			}
			
			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
			
	}
}
