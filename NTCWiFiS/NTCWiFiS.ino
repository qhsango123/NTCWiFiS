extern "C"{
  #include "circular_buffer.h"
}
#include "BoardCFG.h"
/*ADC Variables*/
uint16_t ADC_Value; /*ADC value Read from ADC_pin ( analog mltiplexer)*/
uint16_t NTC1_ADC_Val;  /*NTC1 ADC value Read from NTC1_Pin or analog mltiplexer (ouput SB2 = 0, SB1 = 0, SB0 = 0) */
uint16_t NTC2_ADC_Val;  /*NTC2 ADC value Read from NTC2_Pin or analog mltiplexer (ouput SB2 = 0, SB1 = 0, SB0 = 1) */
uint16_t NTC3_ADC_Val;  /*NTC3 ADC value Read from NTC3_Pin or analog mltiplexer (ouput SB2 = 0, SB1 = 1, SB0 = 0) */
uint16_t NTC4_ADC_Val;  /*NTC4 ADC value Read from NTC4_Pin or analog mltiplexer (ouput SB2 = 0, SB1 = 1, SB0 = 1) */
uint16_t PS1_ADC_Val;   /*PS1 ADC value Read from PS1_Pin or analog mltiplexer (ouput SB2 = 1, SB1 = 0, SB0 = 0) */
uint16_t PS2_ADC_Val;   /*PS2 ADC value Read from PS2_Pin or analog mltiplexer (ouput SB2 = 1, SB1 = 0, SB0 = 1) */
uint16_t VREF_ADC_Val;  /*VREF ADC value Read from analog mltiplexer (ouput SB2 = 1, SB1 = 1, SB0 = 1) */
float NTC1, NTC2, NTC3, NTC4, PS1, PS2, I1, I2;
//float V; /* điện áp 2 đầu NTC */
//float V1; /* điện áp 2 đầu trở 10k */
//float A1; /* dòng điền 2 đầu trở 10k = I */
float R; /* điện trở NTC tại thời điểm đó*/
float Tkevin; /* nhiệt độ đơn vị Kevin */
float Tc; /* nhiệt độ đơn vị độ C */
//int          U  =  3.282;              /* điện áp nguồn */
const int    COUNT_NUMBER    = 20; /* số lần chia */
const int    SAMPLE_NUMBER   = 64; /* số lần lấy mẫu */
int   adcSamples[SAMPLE_NUMBER];  /* chứa các mẫu */
int adc[20]; /* tính trung bình  */
float allADC; /* tổng giá trị adc thu được */


int getMostPopularElement(int arr[], const int n)
{
    int count = 1, tempCount;
    int temp = 0,i = 0,j = 0;
    //Get first element
    int popular = arr[0];
    for (i = 0; i < (n- 1); i++)
    {
        temp = arr[i];
        tempCount = 0;
        for (j = 1; j < n; j++)
        {
            if (temp == arr[j])
                tempCount++;
        }
        if (tempCount > count)
        {
            popular = temp;
            count = tempCount;
        }
    }
    return popular;
}

/***************/
void ADC_Process(void * parameter)
{
  vTaskDelay(1000 / portTICK_PERIOD_MS); /*Delay 1000ms*/
  UART_Debug.printf("ADC_Process is running on CPU %d\n", xPortGetCoreID());
  for(;;)
  {
    //Lấy mẫu
    for (int j=0; j < COUNT_NUMBER; j++)
    {
      for (int i = 0; i < SAMPLE_NUMBER; i++) 
      {
        adcSamples[i] = analogRead(NTC1_Pin);
//        Serial.println(adcSamples[i]);
//        delay(100);
        
  //      NTC1_ADC_Val = analogRead(NTC1_Pin);
        NTC2_ADC_Val = analogRead(NTC2_Pin);
        NTC3_ADC_Val = analogRead(NTC3_Pin);
        NTC4_ADC_Val = analogRead(NTC4_Pin);
        PS1_ADC_Val = analogRead(PS1_Pin);
        PS1_ADC_Val = analogRead(PS2_Pin);
  //      delay(5);        // wait 5 milliseconds
      }
      //Tìm giá trị xuất hiện nhiều nhất
      adc[j] = getMostPopularElement(adcSamples, SAMPLE_NUMBER);
    }

    

    //Tính giá trị trung bình
    for (int i=0; i< COUNT_NUMBER; i++)
    {
      allADC = allADC + adc[i];
    }
    NTC1_ADC_Val=allADC/COUNT_NUMBER;
    //reset biến cho lần đọc tiếp theo
    allADC=0; 

    //Tính điện áp qua ntc
//    V = ((float)NTC1_ADC_Val*U/4095); //ADC 12 bit
//    Serial.print("Voltage = ");
//    Serial.print(V);
//    Serial.println(" V");

//    //điện áp qua trở 10k
//    V1=U-V;
//    //tính dòng điện
//    A1=V1/10000;
//    //tính trở ntc
//    R2 = (V/(A1))-560;

    /* 
     * điên áp qua NTC và trở 560 là U1= ADC*3.3/4095
     * điện áp qua trở 10k là U2= 3.3-U1 <=> 3.3*(1 - ADC/4095)
     * dòng điện qua NTC và trở 560 bằng với dòng qua trở 10k do mắc nối tiếp
     * => I=I1=I2= (3.3*(1-ADC/4095))/10000 = (3.3*ADC)/(4095*(R+560))  
     * R ở đây là trở kháng của NTC, ADC là giá trị adc đọc được (NTC1_ADC_Val)
     * bỏ 3.3 ở 2 vế biến đổi có được công thức ở dưới
     */
    R=((float)NTC1_ADC_Val*4095*10000)/(4095*(4095-(float)NTC1_ADC_Val))-560;

    
    Serial.print("Resistance = ");
    Serial.print(R);
    Serial.println(" ohm");


    //Tính nhiệt độ, beta=3975, R=100k at 25*C
    Tkevin=(3975*(273.15+25))/(3975+((273.15+25)*log(R/100000)));
    Tc=Tkevin-273.15;
    
    Serial.print("Temperture = ");
    Serial.print(Tc);
    Serial.println("*C");

    Serial.println("");
//    delay(1000);
    
    vTaskDelay(1000 / portTICK_PERIOD_MS); /*Delay 1000ms*/
  }
}
void setup() {
  // put your setup code here, to run once:
  UART_Debug.begin(115200);
  delay(500);
  UART_Debug.println("Hello World");
  UART_Debug.printf("Setup is running on CPU %d\n", xPortGetCoreID());
  xTaskCreatePinnedToCore(
    ADC_Process,
    "ADC_Process",  // Task name
    5000,             // Stack size (bytes)
    NULL,             // Parameter
    1,                // Task priority
    NULL,             // Task handle
    1    // CPU ID
  );
 
}

void loop() {
  // put your main code here, to run repeatedly:
//  UART_Debug.printf("NTC %f\n",NTC1,NTC2,NTC3,NTC4);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  
}
