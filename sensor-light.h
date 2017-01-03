#ifndef __SENSOR_LIGHT
#define __SENSOR_LIGHT

/*standard include*/
#include <pic.h>		// ヘッダ・ファイル
#include <stdio.h>

/*user include*/


/*ピックマイコン内部クロック周波数4MHz(pic12f675)*/
#define _XTAL_FREQ 4000000	//使用する周波数に応じた値をセット(delayマクロで使用)

/*ピン設定 pic12f675*/
#define	LED		GPIO5
#define	SENSOR	GPIO4

/*定数設定*/
#define MAX_PWM_STAGE 1024 //PWMの段階
#define TMR0_VAL      0x10 //タイマー割り込み時間
                           // タイマ0を16uｓにセット(内部クロック4m(0.25us)*4(内部) * 16)

#define meas_num 6          //計測回数
#define step 10             // 輝度調節を滑らかにするための調節段階
#define Removal_range 10    //この数値以下の場合ノイズとしてシャットアウト

#define rounding_max 240    //この数値以上なら明るさ 100%
#define rounding_min 20     //この数値以下なら明るさ 0%

/*定義*/
#define HIGH        1
#define LOW         0
#define TRUE        1
#define FALSE       0
#define ERR_RET    -1  //異常終了
#define Normal_RET  0   //正常終了

/*デバッグ*/
#define debug //デバッグ時は定義する
#ifdef  debug
    #include "putch.h"
#endif

__CONFIG(CPD_OFF & CP_OFF & BOREN_OFF & MCLRE_OFF & PWRTE_ON & WDTE_OFF & FOSC_INTRCIO);
// コンフィギュレーション・ビットの設定
// EEPROMコードプロテクト=OFF
// プログラムコード・プロテクト=OFF
// ブラウンアウトリセット=OFF
// GP3/MCLRピン=GP3
// パワーアップ・タイマ=ON
// ウォッチドッグ・タイマ=OFF
// INTIO発振(4MHz)

/*プロトタイプ宣言*/
void ioport(void);		// I/Oポート設定関数のプロトタイプ
unsigned char AD_CONVERT(unsigned char CH1,unsigned char CH0); //A-D変換
unsigned char noise_removal(unsigned char meas_val); //ノイズ除去
void brightness_adjustment(unsigned char meas_val); //輝度調節を滑らかにする
int confirmation(unsigned char meas_val);

#endif
