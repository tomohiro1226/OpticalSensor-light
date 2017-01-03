/*header include*/
#include "sensor-light.h"

/*グローバル変数宣言*/
unsigned int g_out_led = 0; //PWM制御を行うときの段階値を格納
                          //これの値を変えることで、出力(LEDの明るさ)が変わる

void main(void)
{
    unsigned char wire;

	OSCCAL=__osccal_val();		//　キャリブレーションデータの書込み

	ioport();

	while(TRUE){
        wire = noise_removal(AD_CONVERT(0, 0)); //AN0(センサー)測定
                                                //ノイズ除去
        #ifdef debug
        printf("AD = %x\n\r", wire);
        #endif

        brightness_adjustment(wire); //輝度を滑らかに調節する
	}
}

void ioport(void)
{
	GPIO = 0x00; //GPIO <- 0

/*-----------------------------------------------------------------*/
	TRISIO = 0x04; //入出力設定 GP2を入力

	ANSEL &= 0xF1; //GP1,GP2,GP4をデジタルI/Oに設定
/*-----------------------------------------------------------------*/

	CMCON |= 0x07; // Comparator Off

	OPTION_REG=0x48;	// タイマ0のクロックは内部クロック、プリスケーラは使わない

	INTCON = 0xA0;	// GIE,PEIE,T0IE,INTE,GPIE,T0IF,INTF,GPIF
                    // GIEとT0IEのみ'1'

    TMR0=TMR0_VAL;
}

void interrupt isr(void)	// 割り込み関数
{
	static unsigned char   stage_cnt = 0;

	if(T0IF & T0IE){ //タイマー割り込み？
		if(cnt == MAX_PWM_STAGE){//リセット
			stage_cnt = 0;
			LED = HIGH;
		}

        /*PWMを作り出している*/
        if( g_out_led == stage_cnt )LED = LOW;
        stage_cnt ++;

	}



	T0IF=0;			// タイマ0割り込みフラグを0にする
	TMR0=TMR0_VAL;
}

unsigned char AD_CONVERT(unsigned char CH1,unsigned char CH0)
{
    unsigned char cnt=meas_num;
    unsigned short  meas_val=0; //計測結果を格納

	CHS0=CH0;			//ピンの電圧をAD変換する
	CHS1=CH1;			//

    while(cnt--){
        __delay_us(30);		//30us待つ

        GO_DONE = HIGH;     //変換開始

        while(GO_DONE);		//変換終了まで待つ(GODONE=0)

        meas_val += (unsigned short)ADRESH; //8bit計測結果を足し合わせる
    }

	return(unsigned char)(meas_val / meas_num);
}

/*センサーの計測値が　Removal_range 以下の変化量ならノイズと判定*/
unsigned char noise_removal(unsigned char meas_val){

    static unsigned char old_meas_val;

    if( old_meas_val >= (meas_val+Removal_range) ){
        old_meas_val = meas_val;
    }

    if( old_meas_val <= (meas_val-Removal_range) ){
        old_meas_val = meas_val;
    }

    return old_meas_val;
}

/*LEDのPWM値を step段階に分けて、調節する。 そのため、滑らかになる*/
void brightness_adjustment(unsigned char meas_val){

    char x;
    unsigned char i = step; //輝度調節の段階

    if( confirmation(meas_val) )return; //値を変化させない場合リターン

     if( meas_val  > (unsigned char)g_out_led ){ // 大きかった場合(引数のほうが)
         x = (meas_val - g_out_led) / step;
         g_out_led = g_out_led + ((meas_val - g_out_led) % step); //最大step値の誤差が起きるので、それを解消する
     }

     if( meas_val < (unsigned char)g_out_led ){ // 小さかった場合(引数の方が)
         x = (g_out_led - meas_val) / step * -1;
         g_out_led = g_out_led - ((g_out_led - meas_val) % step); //最大step値の誤差が起きるので、それを解消する
     }

    while(i--){
        g_out_led += x;  //step段階の変化量を足し合わせる
        __delay_us(500);
    }
}

int confirmation(unsigned char meas_val){

    if( meas_val > rounding_max )return 1; //以上(一定値)なら　変化させない
                                           //光る、光らないの変化をしっかり創るため

    if( meas_val < rounding_min )return 1; //以下(一定値)なら　変化させない

    if(g_out_led == meas_val)return 1;//値に変化が無かったらリターン
                                     //ここで戻るのは、ノイズ除去ルーチンが働いた時

    return 0;
}
