//controlling the led template


const u16 led_num = 8; //defining the led number
const u16 stage_NUM = 6;
const u16 stage_sec[] = {5,10,15,20,25,30};
//defining speed for each stage,unit:nodes per minutes
const u16 reflect_time = 100;  //100ms


const u16 multiplier = 20;   //defining game speed, 1000 is the fastest
const u16 delay_time_led = 100; //100ms to delay LED
const u16 delay_table[] = {200,100,67,50,40,33};
const u16 score_init = 5;	
const u16 opt_stage[] = {1000,2500,5000,8000,13000,20000};
void init_game();

int generate_number();   //get a randomly chosen 0-7

void Whac_A_Mole_main();  //main function
void extra_stage();   //stage 6
void alarm_bells();

void victory_sound();

void put_LED_on(u16 label);
void LED_all_on();
void LED_disblack();
void game_start();
void game_over();
void clean_stage();