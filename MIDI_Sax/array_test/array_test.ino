#define breath_len 8
int breath_array[breath_len];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int breath_sum = 0, breath_out = 0;
  int n=0;
  //Shift the array
  for (n=0;n<breath_len-1;n++){
    *(breath_array+n) = *(breath_array+n+1);
  }
  //Add the new value
  *(breath_array+breath_len-1) = analogRead(A0);
  //print the array
  for (n=0;n<8;n++){
    breath_sum = breath_sum + *(breath_array+(n));
  }
  breath_out = breath_sum/8;
  Serial.print(breath_out);
  Serial.print("\t");
  Serial.print(*(breath_array+breath_len-1));
  Serial.print("\n");
  delay(5);
}
