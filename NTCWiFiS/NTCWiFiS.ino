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
float V; /* điện áp 2 đầu NTC */
float V1; /* điện áp 2 đầu trở 10k */
float A1; /* dòng điền 2 đầu trở 10k = I */
float R; /* điện trở NTC tại thời điểm đó*/
float Tkevin; /* nhiệt độ đơn vị Kevin */
float Tc; /* nhiệt độ đơn vị độ C */
const int    SAMPLE_NUMBER      = 64; /* số lần lấy mẫu */
int    adcSamples[SAMPLE_NUMBER];  /* chứa các mẫu */


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
    for (int i = 0; i < SAMPLE_NUMBER; i++) 
    {
      adcSamples[i] = analogRead(NTC1_Pin);
      
//      NTC1_ADC_Val = analogRead(NTC1_Pin);
      NTC2_ADC_Val = analogRead(NTC2_Pin);
      NTC3_ADC_Val = analogRead(NTC3_Pin);
      NTC4_ADC_Val = analogRead(NTC4_Pin);
      PS1_ADC_Val = analogRead(PS1_Pin);
      PS1_ADC_Val = analogRead(PS2_Pin);
      delay(5);        // wait 5 milliseconds
    }

    //Tìm giá trị xuất hiện nhiều nhất
    NTC1_ADC_Val = getMostPopularElement(adcSamples, SAMPLE_NUMBER);
//    Serial.println(NTC1_ADC_Val);
//    for(int i=0; i<SAMPLE_NUMBER; i++)
//    {
//      Serial.print(adcSamples[i]);
//      Serial.print(" ");
//    }   
    V = ((float)NTC1_ADC_Val*3.3/4095); //ADC 12 bit
    Serial.print("Voltage= ");
    Serial.print(V);
    Serial.println(" V");

    V1=3.3-V;
    A1=V1/10000;
    
    R = (V/(A1))-560;

    Serial.print("Resistance= ");
    Serial.print(R);
    Serial.println(" ohm");

    //Tính nhiệt độ
    Tkevin=(3975*(273.15+25))/(3975+((273.15+25)*log(R/100000)));
    Tc=Tkevin-273.15;
    Serial.print("Temperture: ");
    Serial.print(Tc);
    Serial.println("*C");

    Serial.println("");
    delay(2000);
    
//    vTaskDelay(1000 / portTICK_PERIOD_MS); /*Delay 1000ms*/
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
