/* This file is meant to be used with the SCILAB arduino  
2	   toolbox, however, it can be used from the IDE environment
3	   (or any other serial terminal) by typing commands like:
4	   
5	   Conversion ascii -> number
6	   48->'0' ... 57->'9' 58->':' 59->';' 60->'<' 61->'=' 62->'>' 63->'?' 64->'@' 
7	   65->'A' ... 90->'Z' 91->'[' 92->'\' 93->']' 94->'^' 95->'_' 96->'`' 
8	   97->'a' ... 122->'z'
9	   
10	   Dan0 or Dan1 : attach digital pin n (ascii from 2 to b) to input (0) or output (1)
11	   Drn : read digital value (0 or 1) on pin n (ascii from 2 to b)
12	   Dwn0 or Dwn1 : write 1 or 0 on pin n
13	   An    : reads analog pin n (ascii from 0 to 19)
14	   Wnm  : write analog value m (ascii from 0 to 255) on pin n (ascii from 0 to 19)
15	   Sa1 or Sa2 : Attach servo 1 (digital pin 9) or 2 (digital pin 10)
16	   Sw1n or Sw2n : moves servo 1 or servo 2 to position n (from ascii(0) to ascii(180))
17	   Sd1 or Sd2 : Detach servo 1 or 2
18	   
19	   Generic DC_Motor
20	   Cijkl : setup for generic DCmotor number i (1 to 4), PW1 on pin number j, PWM2 or direction on pin number k, mode=l
21	           l=0 for L293 (2 PWM) and l=1 for L298 (1PWM + 1 bit for direction)
22	   Mijk  : sets speed for generic DCmotor number i, j=0/1 for direction, k=ascii(0) .. ascii(255)
23	   Mir   : releases motor i (r=release)
24	
25	   Generic Interrupt counter
26	   Iai   : activate counter on INT number i (i=ascii(2 or 3 or 18 or 19 or 20 or 21)
27	   Iri   : release counter on INT number i
28	   Ipi   : read counter on INT number i
29	   Izi   : reset counter on INT number i
30	
31	   Generic Encoder
32	   Eajkl: activate encoder on channelA on INT number j (j=ascii(2 or 3 or 18 or 19 or 20 or 21) et channelB on pin k or INT number k (k=ascii(0)..ascii(53))
33	           and l=1 or 2 or 4 for 1x mode (count every rising of chA) or 2x mode (count every change statement of chA)
34	           or  4x mode (every change statement of chA et chB)
35	   Eri   : release encoder on INTi
36	   Epi   : read position of encoder on INTi
37	   Ezi   : reset value of encoder on INTi position
38	   
39	   R0    : sets analog reference to DEFAULT
40	   R1    : sets analog reference to INTERNAL
41	   R2    : sets analog reference to EXTERNAL
42	
43	   */
	
	#include <Servo.h>
	
	/* define internal for the MEGA as 1.1V (as as for the 328)  */
	#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
49	#define INTERNAL INTERNAL1V1
50	#endif
51	
52	/* create and initialize servos                              */
53	Servo servo1;
54	Servo servo2;
55	
56	/* Generic motors */
57	int dcm1_pin1,dcm1_pin2,dcm1_mode;
58	int dcm2_pin1,dcm2_pin2,dcm2_mode;
59	int dcm3_pin1,dcm3_pin2,dcm3_mode;
60	int dcm4_pin1,dcm4_pin2,dcm4_mode;
61	
62	// Generic encoder 
63	/* Encoders initialisation */
64	// volatile declare as those variables will change in interrupts
65	volatile long int encoder_0_position = 0,encoder_1_position = 0, encoder_2_position = 0, encoder_3_position = 0, encoder_4_position = 0, encoder_5_position = 0;
66	int encoder_0_int2 ;          // Pin used for encoder0 chanel B : define from scilab
67	int encoder_1_int2 ;          // Pin used for encoder1 chanel B : define from scilab
68	int encoder_2_int2 ;          // Pin used for encoder2 chanel B : define from scilab
69	int encoder_3_int2 ;          // Pin used for encoder3 chanel B : define from scilab
70	int encoder_4_int2 ;          // Pin used for encoder4 chanel B : define from scilab
71	int encoder_5_int2 ;          // Pin used for encoder5 chanel B : define from scilab
72	int encoder_num, encoder_int2;
73	int corresp[6]={2,3,21,20,19,18}; //Correspondance beetween interrupt number and pin number
74	
75	//Generic counter
76	volatile long int counter_0=0,counter_1=0,counter_2=0,counter_3=0,counter_4=0,counter_5=0;
77	
78	int initiat=1;
79	
80	void setup() {
81	  /* initialize serial                                       */
82	  Serial.begin(115200);
83	  
84	}
85	
86	void loop() {
87	  
88	  /* variables declaration and initialization                */
89	  
90	  static int  s   = -1;    /* state                          */
91	  static int  pin = 13;    /* generic pin number             */
92	  static int  dcm =  4;    /* generic dc motor number        */
93	
94	  int  val =  0;           /* generic value read from serial */
95	  int  agv =  0;           /* generic analog value           */
96	  int  dgv =  0;           /* generic digital value          */
97	  static int  enc   = 1;    /* encoder number 1 (or 2 for Arduino mega)     */
98	
99	  while (Serial.available()==0) {}; // Waiting char
100	  val = Serial.read(); 
101	  
102	//  if (val==0){// version
103	//    Serial.print('v3');
104	//    val=-1;
105	//  }
106	  //case A -> Analog
107	  if (val==65){//A -> Analog read
108	    while (Serial.available()==0) {}; // Waiting char
109	//    val=Serial.read();
110	//    if (val==114){ //'r'-> read pin
111	//       while (Serial.available()==0) {}; // Waiting char
112	       val=Serial.read();
113	       if (val>47 && val<67) { //from pin 0, to pin 19
114	          pin=val-48; //number of the pin
115	          agv=analogRead(pin);
116	          //Serial.println(agv);
117	          Serial.write((uint8_t*)&agv,2); /* send binary value via serial  */   
118	       }
119	       val=-1;
120	  }
121	  else if (val==87){//W -> Analog write
122	      while (Serial.available()==0) {}; // Waiting char
123	      val=Serial.read();
124	         if (val>47 && val<67) { //from pin 0 to pin 19
125	            pin=val-48; //number of the pin
126	            while (Serial.available()==0) {}; // Waiting char
127	            val=Serial.read();
128	            analogWrite(pin,val);
129	         }
130	          val=-1;
131	      }
132	  //}
133	
134	  //case D -> Digital
135	  else if (val==68){//D -> Digital pins
136	    while (Serial.available()==0) {}; // Waiting char
137	    val=Serial.read();
138	    if (val==97){ //'a'-> declare pin
139	       while (Serial.available()==0) {}; // Waiting char
140	       val=Serial.read();
141	       if (val>49 && val<102) {
142	         pin=val-48;
143	         while (Serial.available()==0) {}; // Waiting char
144	         val=Serial.read();
145	         if (val==48 || val==49) { 
146	            if (val==48){//'0' -> input
147	               pinMode(pin,INPUT);
148	            }
149	            else if (val==49){//'1' -> output
150	               pinMode(pin,OUTPUT);
151	            }
152	         }
153	       }
154	    }
155	    if (val==114){ //'r'-> read pin
156	       while (Serial.available()==0) {}; // Waiting char
157	       val=Serial.read();
158	       if (val>49 && val<102) { 
159	          pin=val-48; //number of the digital pin
160	          dgv=digitalRead(pin);      
161	//          Serial.println(dgv);        
162	          Serial.print(dgv);         
163	       }
164	    }
165	    if (val==119){ //'w'-> write pin
166	       while (Serial.available()==0) {}; // Waiting char
167	       val=Serial.read();
168	       if (val>49 && val<102) { 
169	          pin=val-48; //number of the digital pin
170	          while (Serial.available()==0) {}; // Waiting char
171	          val=Serial.read();
172	          if (val==48 || val==49) { // 0 or 1
173	            dgv=val-48;
174	            digitalWrite(pin,dgv);
175	//            Serial.println(dgv);    
176	          }
177	       }
178	    }
179	   val=-1;
180	
181	  }
182	  //case S -> servomotor
183	  else if (val==83){
184	    while (Serial.available()==0) {}; // Waiting char
185	    val=Serial.read();
186	    if (val==97){ //'a'-> declare servo
187	       while (Serial.available()==0) {}; // Waiting char
188	       val=Serial.read();
189	       if (val==49 || val==50) { //servo 1 or 2
190	          pin=val-48; //number of the servo
191	          if (pin==1) {
192	            servo1.attach(9); 
193	            servo1.write(0);           
194	//            agv=servo1.read();
195	//            Serial.println(agv);
196	          }
197	          if (pin==2) {
198	            servo2.attach(10); 
199	            servo2.write(0);           
200	//            agv=servo2.read();
201	//            Serial.println(agv);
202	          }
203	
204	       }
205	    }
206	    if (val==100){ //'d'-> detach servo
207	       while (Serial.available()==0) {}; // Waiting char
208	       val=Serial.read();
209	       if (val==49 || val==50) { //servo 1 or 2
210	          pin=val-48; //number of the servo
211	          if (pin==1) {servo1.detach(); }
212	          if (pin==2) {servo2.detach(); }
213	       }
214	    }
215	    if (val==119){ //'w'-> write pin
216	       while (Serial.available()==0) {}; // Waiting char
217	       val=Serial.read();
218	       if (val==49 || val==50) { //servo 1 or 2
219	          pin=val-48; //number of the servo
220	          while (Serial.available()==0) {}; // Waiting char
221	          val=Serial.read();
222	          if (val>=0 && val<=180){
223	              if (pin==1) {
224	                servo1.write(val);
225	//                agv=servo1.read();
226	//                Serial.println(agv);
227	              }
228	              if (pin==2) {
229	                servo2.write(val);
230	//                agv=servo2.read();
231	//                Serial.println(agv);
232	              }
233	          }
234	       }
235	    }
236	   val=-1;
237	
238	  }
239	  
240	  //case I -> Interrupt
241	  else if (val==73){
242	      /* ASKING ACTIVATION OF AN COUNTER     */
243	      while (Serial.available()==0) {}; // Waiting char
244	      val=Serial.read();                                
245	      if (val==97) { //a = activation
246	         while (Serial.available()==0) {};                 // Waiting char
247	         val=Serial.read();                                // Read int_number (must be 0 or 1 on UNO / 1 to 5 on MEGA) : int_number set to encoder number
248	         pinMode(corresp[val],INPUT);              // set interrupt pin as input
249	         if       (val == 0) {attachInterrupt(val, counter_0_change, RISING);counter_0=0;}  //counter INT0
250	         else if  (val == 1) {attachInterrupt(val, counter_1_change, RISING);counter_1=0;}  //counter INT1
251	         else if  (val == 2) {attachInterrupt(val, counter_2_change, RISING);counter_2=0;}  //counter INT2
252	         else if  (val == 3) {attachInterrupt(val, counter_3_change, RISING);counter_3=0;}  //counter INT3
253	         else if  (val == 4) {attachInterrupt(val, counter_4_change, RISING);counter_4=0;}  //counter INT4
254	         else if  (val == 5) {attachInterrupt(val, counter_5_change, RISING);counter_5=0;}  //counter INT5
255	      }
256	      /* ASKING POSITION OF A COUNTER     */ 
257	      if (val==112) { //p = sending counting value
258	         while (Serial.available()==0) {};   // Waiting char
259	         val = Serial.read()   ;             //reading next value = counter number
260	         if      (val==0){ Serial.write((uint8_t*)&counter_0,4); }// asking counter 0 
261	         else if (val==1){ Serial.write((uint8_t*)&counter_1,4); }// asking counter 1 
262	         else if (val==2){ Serial.write((uint8_t*)&counter_2,4); }// asking counter 2 
263	         else if (val==3){ Serial.write((uint8_t*)&counter_3,4); }// asking counter 3 
264	         else if (val==4){ Serial.write((uint8_t*)&counter_4,4); }// asking counter 4 
265	         else if (val==5){ Serial.write((uint8_t*)&counter_5,4); }// asking counter 5 
266	     }
267	      /* ASKING RELEASE OF AN INTERRUPT   */ 
268	      if (val==114) { //r = release counter 
269	         while (Serial.available()==0) {};        // Waiting char
270	         val = Serial.read();                     //reading next value = counter number
271	         detachInterrupt(val);                    // Detach interrupt on chanel A of counter num=val
272	         if (val==0)       { counter_0=0;}        // Reset counter
273	         else if (val==1)  { counter_1=0;}        // Reset counter
274	         else if (val==2)  { counter_2=0;}        // Reset counter
275	         else if (val==3)  { counter_3=0;}        // Reset counter
276	         else if (val==4)  { counter_4=0;}        // Reset counter
277	         else if (val==5)  { counter_5=0;}        // Reset counter
278	      }
279	      /* ASKING RESET VALUE OF AN COUNTER     */ 
280	      if (val==122) { //z set to zero
281	         while (Serial.available()==0) {};        // Waiting char
282	         val = Serial.read();                     //reading next value = counter number
283	         if (val==0)       { counter_0=0;}        // Reset counter
284	         else if (val==1)  { counter_1=0;}        // Reset counter
285	         else if (val==2)  { counter_2=0;}        // Reset counter
286	         else if (val==3)  { counter_3=0;}        // Reset counter
287	         else if (val==4)  { counter_4=0;}        // Reset counter
288	         else if (val==5)  { counter_5=0;}        // Reset counter
289	       }
290	    val=-1;
291	
292	  } 
293	  
294	  //case E -> Encoder
295	  else if (val==69){    
296	      /*Generic encoder functions */
297	      while (Serial.available()==0) {};
298	      val=Serial.read();  
299	      /* ASKING ACTIVATION OF AN ENCODER     */
300	      if (val==97) {                                       //activation
301	         while (Serial.available()==0) {};                 // Waiting char
302	         encoder_num=Serial.read();                        // Read int_number (must be 0 or 1 on UNO / 1 to 5 on MEGA) : int_number set to encoer number
303	         pinMode(corresp[encoder_num],INPUT);              // set interrupt pin as input
304	         while (Serial.available()==0) {};                 // Waiting char
305	         encoder_int2=Serial.read();                       // Read int2 (must be a digital PIN with interrupt or not : depends on mode)
306	                                                           // no declaration for the moment : wait for encoder mode
307	         while (Serial.available()==0) {};                 // Waiting char
308	         int mode = Serial.read()-48;                      // Read mode 1 ou 2 (1 counting only rising of chA, 2 counting rising and falling)
309	         if (mode == 4) {                                  // mode 4x : 2 cases : chA=pin2 / chB=pin3 or chA=pin3/chB=pin2 [Uno retriction]
310	            pinMode(corresp[encoder_int2],INPUT);          // set interrupt number as input
311	         } else {
312	            pinMode(encoder_int2,INPUT);                   // set pin as input
313	         }
314	         
315	         if (encoder_num == 0) {                             //encoder INT0
316	           encoder_0_position=0;                             // Reset position
317	           if (mode==4) {
318	             encoder_0_int2=corresp[encoder_int2];           // Save pin of second interruption
319	             attachInterrupt(encoder_num , encoder_change_m4_A0, CHANGE); // Attach interrupt on chanel A change
320	             attachInterrupt(encoder_int2, encoder_change_m4_B0, CHANGE); // Attach interrupt on chanel B change
321	           } else if (mode==2) {
322	             encoder_0_int2=encoder_int2;
323	             attachInterrupt(encoder_num, encoder_0_change_m2, CHANGE); // Attach interrupt on chanel A change
324	           } else if (mode==1) {
325	             encoder_0_int2=encoder_int2;
326	             attachInterrupt(encoder_num, encoder_0_change_m1, RISING); // Attach interrupt on chanel A rising
327	           }
328	         } else if (encoder_num == 1) {                      //encoder INT1
329	           encoder_1_position=0;                             // Reset position
330	           if (mode==4) {
331	             encoder_1_int2=corresp[encoder_int2];           // Save pin of second interruption
332	             attachInterrupt(encoder_num , encoder_change_m4_A1, CHANGE); // Attach interrupt on chanel A change
333	             attachInterrupt(encoder_int2, encoder_change_m4_B1, CHANGE); // Attach interrupt on chanel B change
334	           } else if (mode==2) {
335	             encoder_1_int2=encoder_int2;
336	             attachInterrupt(encoder_num, encoder_1_change_m2, CHANGE); // Attach interrupt on chanel A change
337	           } else if (mode==1) {
338	             encoder_1_int2=encoder_int2;
339	             attachInterrupt(encoder_num, encoder_1_change_m1, RISING); // Attach interrupt on chanel A rising
340	           }
341	         } else if (encoder_num == 2) {                      //encoder INT2
342	           encoder_2_position=0;                             // Reset position
343	           if (mode==4) {
344	             encoder_2_int2=corresp[encoder_int2];           // Save pin of second interruption
345	             attachInterrupt(encoder_num , encoder_change_m4_A2, CHANGE); // Attach interrupt on chanel A change
346	             attachInterrupt(encoder_int2, encoder_change_m4_B2, CHANGE); // Attach interrupt on chanel B change
347	           } else if (mode==2) {
348	             encoder_2_int2=encoder_int2;
349	             attachInterrupt(encoder_num, encoder_2_change_m2, CHANGE); // Attach interrupt on chanel A change
350	           } else if (mode==1) {
351	             encoder_2_int2=encoder_int2;
352	             attachInterrupt(encoder_num, encoder_2_change_m1, RISING); // Attach interrupt on chanel A rising
353	           }
354	         } else if (encoder_num == 3) {                      //encoder INT3
355	           encoder_3_position=0;                             // Reset position
356	           if (mode==4) {
357	             encoder_3_int2=corresp[encoder_int2];           // Save pin of second interruption
358	             attachInterrupt(encoder_num , encoder_change_m4_A3, CHANGE); // Attach interrupt on chanel A change
359	             attachInterrupt(encoder_int2, encoder_change_m4_B3, CHANGE); // Attach interrupt on chanel B change
360	           } else if (mode==2) {
361	             encoder_3_int2=encoder_int2;
362	             attachInterrupt(encoder_num, encoder_3_change_m2, CHANGE); // Attach interrupt on chanel A change
363	           } else if (mode==1) {
364	             encoder_3_int2=encoder_int2;
365	             attachInterrupt(encoder_num, encoder_3_change_m1, RISING); // Attach interrupt on chanel A rising
366	           }
367	         } else if (encoder_num == 4) {                      //encoder INT4
368	           encoder_4_position=0;                             // Reset position
369	           if (mode==4) {
370	             encoder_4_int2=corresp[encoder_int2];           // Save pin of second interruption
371	             attachInterrupt(encoder_num , encoder_change_m4_A4, CHANGE); // Attach interrupt on chanel A change
372	             attachInterrupt(encoder_int2, encoder_change_m4_B4, CHANGE); // Attach interrupt on chanel B change
373	           } else if (mode==2) {
374	             encoder_4_int2=encoder_int2;
375	             attachInterrupt(encoder_num, encoder_4_change_m2, CHANGE); // Attach interrupt on chanel A change
376	           } else if (mode==1) {
377	             encoder_4_int2=encoder_int2;
378	             attachInterrupt(encoder_num, encoder_4_change_m1, RISING); // Attach interrupt on chanel A rising
379	           }
380	         } else if (encoder_num == 5) {                      //encoder INT5
381	           encoder_5_position=0;                             // Reset position
382	           if (mode==4) {
383	             encoder_5_int2=corresp[encoder_int2];           // Save pin of second interruption
384	             attachInterrupt(encoder_num , encoder_change_m4_A5, CHANGE); // Attach interrupt on chanel A change
385	             attachInterrupt(encoder_int2, encoder_change_m4_B5, CHANGE); // Attach interrupt on chanel B change
386	           } else if (mode==2) {
387	             encoder_5_int2=encoder_int2;
388	             attachInterrupt(encoder_num, encoder_5_change_m2, CHANGE); // Attach interrupt on chanel A change
389	           } else if (mode==1) {
390	             encoder_5_int2=encoder_int2;
391	             attachInterrupt(encoder_num, encoder_5_change_m1, RISING); // Attach interrupt on chanel A rising
392	           }
393	         }
394	      }
395	      /* ASKING POSITION OF AN ENCODER     */ 
396	      if (val==112) { //p = sending encoder position
397	         while (Serial.available()==0) {};   // Waiting char
398	         val = Serial.read()   ;             //reading next value = encoder number
399	         if      (val==0){ Serial.write((uint8_t*)&encoder_0_position,4); }// asking encoder 0 position
400	         else if (val==1){ Serial.write((uint8_t*)&encoder_1_position,4); }// asking encoder 1 position
401	         else if (val==2){ Serial.write((uint8_t*)&encoder_2_position,4); }// asking encoder 2 position
402	         else if (val==3){ Serial.write((uint8_t*)&encoder_3_position,4); }// asking encoder 3 position
403	         else if (val==4){ Serial.write((uint8_t*)&encoder_4_position,4); }// asking encoder 4 position
404	         else if (val==5){ Serial.write((uint8_t*)&encoder_5_position,4); }// asking encoder 5 position
405	     }
406	      /* ASKING RELEASE OF AN ENCODER     */ 
407	      if (val==114) { //r = release encoder 
408	         while (Serial.available()==0) {};                 // Waiting char
409	         val = Serial.read();                              //reading next value = encoder number
410	         detachInterrupt(val);                             // Detach interrupt on chanel A of encoder num=val
411	         if (val==0)       { encoder_0_position=0;encoder_0_int2=-1;}        // Reset position
412	         else if (val==1)  { encoder_1_position=0;encoder_1_int2=-1;}        // Reset position
413	         else if (val==2)  { encoder_2_position=0;encoder_2_int2=-1;}        // Reset position
414	         else if (val==3)  { encoder_3_position=0;encoder_3_int2=-1;}        // Reset position
415	         else if (val==4)  { encoder_4_position=0;encoder_4_int2=-1;}        // Reset position
416	         else if (val==5)  { encoder_5_position=0;encoder_5_int2=-1;}        // Reset position
417	         while (Serial.available()==0) {};                 // Waiting char
418	         val = Serial.read();                              // reading next value = encoder number
419	         detachInterrupt(val);                             // Detach interrupt on chanel B of encoder num=val (may be the same if mode=1 or 2)
420	      }
421	      /* ASKING RESET POSITION OF AN ENCODER     */ 
422	      if (val==122) {  // z = encoder position to zero
423	         while (Serial.available()==0) {};                 // Waiting char
424	         val = Serial.read();                              //reading next value = encoder number
425	         if (val==0)       { encoder_0_position=0;}        // Reset position
426	         else if (val==1)  { encoder_1_position=0;}        // Reset position
427	         else if (val==2)  { encoder_2_position=0;}        // Reset position
428	         else if (val==3)  { encoder_3_position=0;}        // Reset position
429	         else if (val==4)  { encoder_4_position=0;}        // Reset position
430	         else if (val==5)  { encoder_5_position=0;}        // Reset position
431	      }
432	     val=-1;
433	
434	  }
435	  
436	  //case C -> DCmotor init
437	  else if(val==67){
438	    while (Serial.available()==0) {};                
439	    val = Serial.read();
440	    /* 2nd char = motor number */
441	    if (val>48 && val<53) {
442	        dcm=val-48;                
443	        while (Serial.available()==0) {};                
444	        val = Serial.read();      
445	        /* the third received value indicates the pin1 number from ascii(2)=50 to ascii(e)=101  */
446	        if (val>49 && val<102) {
447	          if (dcm==1) dcm1_pin1=val-48;/* calculate motor pin1 */
448	          if (dcm==2) dcm2_pin1=val-48;/* calculate motor pin1 */
449	          if (dcm==3) dcm3_pin1=val-48;/* calculate motor pin1 */
450	          if (dcm==4) dcm4_pin1=val-48;/* calculate motor pin1 */
451	          pinMode(val-48, OUTPUT); //set pin as output
452	          analogWrite(val-48,0);   /* DUTY CYCLE */
453	          while (Serial.available()==0) {};                
454	          val = Serial.read();
455	          /* the fourth received value indicates the pin2 number from ascii(2)=50 to ascii(e)=101  */
456	          if (val>49 && val<102) {
457	            if (dcm==1) dcm1_pin2=val-48;/* calculate motor pin2 */
458	            if (dcm==2) dcm2_pin2=val-48;/* calculate motor pin2 */
459	            if (dcm==3) dcm3_pin2=val-48;/* calculate motor pin2 */
460	            if (dcm==4) dcm4_pin2=val-48;/* calculate motor pin2 */
461	            pinMode(val-48, OUTPUT); //set pin as output
462	            while (Serial.available()==0) {};                
463	            val = Serial.read();
464	            /* the fifth received value indicates the pin2 number from ascii(2)=50 to ascii(e)=101 */
465	            if (val>47 && val<50) {
466	              int mode = val-48;
467	              if (dcm==1) dcm1_mode=mode;/* calculate motor mode */
468	              if (dcm==2) dcm2_mode=mode;/* calculate motor mode */
469	              if (dcm==3) dcm3_mode=mode;/* calculate motor mode */
470	              if (dcm==4) dcm4_mode=mode;/* calculate motor mode */
471	              //initialization of port
472	              if(mode==0){//L293
473	                if (dcm==1) analogWrite(dcm1_pin2,0);   /* DUTY CYCLE */
474	                if (dcm==2) analogWrite(dcm2_pin2,0);   /* DUTY CYCLE */
475	                if (dcm==3) analogWrite(dcm3_pin2,0);   /* DUTY CYCLE */
476	                if (dcm==4) analogWrite(dcm4_pin2,0);   /* DUTY CYCLE */
477	              } else if (mode==1) {//L297
478	                if (dcm==1) digitalWrite(dcm1_pin2, LOW);  /* DIRECTION */
479	                if (dcm==2) digitalWrite(dcm2_pin2, LOW);  /* DIRECTION */
480	                if (dcm==3) digitalWrite(dcm3_pin2, LOW);  /* DIRECTION */
481	                if (dcm==4) digitalWrite(dcm4_pin2, LOW);  /* DIRECTION */      
482	              }
483	              Serial.print("OK"); // tell Scilab that motor s initialization finished
484	                                  // Cette commande sert à rien dans la toolbox de base,
485	                                  // sauf si on prévoit d'ajouter des actions à l'init des moteurs
486	                                  // par exemple chercher la position d'origine !
487	            }
488	          }
489	        }
490	    }
491	    val=-1;
492	
493	  }
494	
495	  //case M -> DC motor
496	  else if(val==77){
497	      while (Serial.available()==0) {};                
498	      val = Serial.read();
499	      /* the second received value indicates the motor number
500	         from abs('1')=49, motor1, to abs('4')=52, motor4    */
501	      if (val>48 && val<53) {
502	        dcm=val-48;                /* calculate motor number */
503	        while (Serial.available()==0) {};       // Waiting char
504	        val = Serial.read();  
505	        /* the third received value indicates the sens direction or release*/
506	        if (val==48 || val ==49){
507	            int direction=val-48;
508	            while (Serial.available()==0) {};       // Waiting char
509	            val = Serial.read();                    //reading next value = 0..255 
510	            if (dcm==1){
511	             if(dcm1_mode==0){//L293 
512	              if(direction==1){
513	                analogWrite(dcm1_pin1,val);
514	                analogWrite(dcm1_pin2,0);
515	              } else {
516	                analogWrite(dcm1_pin2,val);
517	                analogWrite(dcm1_pin1,0);
518	              }
519	             } else {//L298
520	              if (direction==0) digitalWrite(dcm1_pin2,LOW);
521	              if (direction==1) digitalWrite(dcm1_pin2,HIGH);
522	              analogWrite(dcm1_pin1,val);
523	             }
524	            }
525	            if (dcm==2){
526	             if(dcm2_mode==0){//L293 
527	              if(direction==1){
528	                analogWrite(dcm2_pin1,val);
529	                analogWrite(dcm2_pin2,0);
530	              } else {
531	                analogWrite(dcm2_pin2,val);
532	                analogWrite(dcm2_pin1,0);
533	              }
534	             } else {//L298
535	              if (direction==0) digitalWrite(dcm2_pin2,LOW);
536	              if (direction==1) digitalWrite(dcm2_pin2,HIGH);
537	              analogWrite(dcm2_pin1,val);
538	             }
539	            }
540	            if (dcm==3){
541	             if(dcm3_mode==0){//L293 
542	              if(direction==1){
543	                analogWrite(dcm3_pin1,val);
544	                analogWrite(dcm3_pin2,0);
545	              } else {
546	                analogWrite(dcm3_pin2,val);
547	                analogWrite(dcm3_pin1,0);
548	              }
549	             } else {//L298
550	              if (direction==0) digitalWrite(dcm3_pin2,LOW);
551	              if (direction==1) digitalWrite(dcm3_pin2,HIGH);
552	              analogWrite(dcm3_pin1,val);
553	             }
554	            }
555	            if (dcm==4){
556	             if(dcm4_mode==0){//L293 
557	              if(direction==1){
558	                analogWrite(dcm4_pin1,val);
559	                analogWrite(dcm4_pin2,0);
560	              } else {
561	                analogWrite(dcm4_pin2,val);
562	                analogWrite(dcm4_pin1,0);
563	              }
564	             } else {//L298
565	              if (direction==0) digitalWrite(dcm4_pin2,LOW);
566	              if (direction==1) digitalWrite(dcm4_pin2,HIGH);
567	              analogWrite(dcm4_pin1,val);
568	             }
569	            }
570	         }
571	         if (val==114){//release motor
572	           if(dcm==1) {
573	           analogWrite(dcm1_pin1,0);
574	           if(dcm1_mode==0)  analogWrite(dcm1_pin2,0); 
575	            }
576	          if(dcm==2) {
577	           analogWrite(dcm2_pin1,0);
578	           if(dcm2_mode==0)  analogWrite(dcm2_pin2,0); 
579	          }
580	          if(dcm==3) {
581	           analogWrite(dcm3_pin1,0);
582	           if(dcm3_mode==0)  analogWrite(dcm3_pin2,0); 
583	          }
584	          if(dcm==4) {
585	           analogWrite(dcm4_pin1,0);
586	           if(dcm4_mode==0)  analogWrite(dcm4_pin2,0); 
587	          }  
588	         }
589	         
590	      }
591	   val=-1;
592	
593	  }
594	
595	
596	  //case R -> Analog reference
597	  if(val==82){
598	    while (Serial.available()==0) {};                
599	    val = Serial.read();
600	    if (val==48) analogReference(DEFAULT);
601	    if (val==49) analogReference(INTERNAL);
602	    if (val==50) analogReference(EXTERNAL);
603	    if (val==51) Serial.print("v3");
604	    val=-1;
605	  }
606	  
607	} /* end loop statement                                      */
608	
609	
610	/**************************************/
611	// Generic interrupt encoder functions//
612	/**************************************/
613	//Encoder on INT0
614	void encoder_0_change_m1() { //encoder0 mode 1x
615	  int chB=digitalRead(encoder_0_int2);
616	  if (!chB) { encoder_0_position++;}
617	  else { encoder_0_position--; }
618	}
619	void encoder_0_change_m2() { //encoder0 mode 2x
620	  int chB=digitalRead(encoder_0_int2);
621	  int chA=digitalRead(corresp[0]);
622	  if ((chA & !chB)|(!chA & chB)) { encoder_0_position++; }
623	  else { encoder_0_position--; }
624	}
625	void encoder_change_m4_A0(){//encoder0 mode 4x chA
626	  int chA=digitalRead(corresp[0]);
627	  int chB=digitalRead(encoder_0_int2);
628	  if ((chA & !chB)|(!chA & chB)) { encoder_0_position++; }
629	  else { encoder_0_position--; }
630	}
631	void encoder_change_m4_B0(){//encoder0 mode 4x chB
632	  int chA=digitalRead(corresp[0]);
633	  int chB=digitalRead(encoder_0_int2);
634	  if ((!chA & !chB)|(chA & chB)) { encoder_0_position++; }
635	  else { encoder_0_position--; }
636	}
637	//Encoder on INT1
638	void encoder_1_change_m1() { //encoder1 mode 1x
639	  int chB=digitalRead(encoder_1_int2);
640	  if (!chB) { encoder_1_position++;}
641	  else { encoder_1_position--; }
642	}
643	void encoder_1_change_m2() { //encoder1 mode 2x
644	  int chB=digitalRead(encoder_1_int2);
645	  int chA=digitalRead(corresp[1]);
646	  if ((chA & !chB)|(!chA & chB)) { encoder_1_position++; }
647	  else { encoder_1_position--; }
648	}
649	void encoder_change_m4_A1(){//encoder1 mode 4x chA
650	  int chA=digitalRead(corresp[1]);
651	  int chB=digitalRead(encoder_1_int2);
652	  if ((chA & !chB)|(!chA & chB)) { encoder_1_position++; }
653	  else { encoder_1_position--; }
654	}
655	void encoder_change_m4_B1(){//encoder1 mode 4x chB
656	  int chA=digitalRead(corresp[1]);
657	  int chB=digitalRead(encoder_1_int2);
658	  if ((!chA & !chB)|(chA & chB)) { encoder_1_position++; }
659	  else { encoder_1_position--; }
660	}
661	//Encoder on INT2
662	void encoder_2_change_m1() { //encoder2 mode 1x
663	  int chB=digitalRead(encoder_2_int2);
664	  if (!chB) { encoder_2_position++;}
665	  else { encoder_2_position--; }
666	}
667	void encoder_2_change_m2() { //encoder2 mode 2x
668	  int chB=digitalRead(encoder_2_int2);
669	  int chA=digitalRead(corresp[2]);
670	  if ((chA & !chB)|(!chA & chB)) { encoder_2_position++; }
671	  else { encoder_2_position--; }
672	}
673	void encoder_change_m4_A2(){//encoder2 mode 4x chA
674	  int chA=digitalRead(corresp[2]);
675	  int chB=digitalRead(encoder_2_int2);
676	  if ((chA & !chB)|(!chA & chB)) { encoder_2_position++; }
677	  else { encoder_2_position--; }
678	}
679	void encoder_change_m4_B2(){//encoder2 mode 4x chB
680	  int chA=digitalRead(corresp[2]);
681	  int chB=digitalRead(encoder_2_int2);
682	  if ((!chA & !chB)|(chA & chB)) { encoder_2_position++; }
683	  else { encoder_2_position--; }
684	}
685	//Encoder on INT3
686	void encoder_3_change_m1() { //encoder3 mode 1x
687	  int chB=digitalRead(encoder_3_int2);
688	  if (!chB) { encoder_3_position++;}
689	  else { encoder_3_position--; }
690	}
691	void encoder_3_change_m2() { //encoder3 mode 2x
692	  int chB=digitalRead(encoder_3_int2);
693	  int chA=digitalRead(corresp[3]);
694	  if ((chA & !chB)|(!chA & chB)) { encoder_3_position++; }
695	  else { encoder_3_position--; }
696	}
697	void encoder_change_m4_A3(){//encoder3 mode 4x chA
698	  int chA=digitalRead(corresp[3]);
699	  int chB=digitalRead(encoder_3_int2);
700	  if ((chA & !chB)|(!chA & chB)) { encoder_3_position++; }
701	  else { encoder_3_position--; }
702	}
703	void encoder_change_m4_B3(){//encoder3 mode 4x chB
704	  int chA=digitalRead(corresp[3]);
705	  int chB=digitalRead(encoder_3_int2);
706	  if ((!chA & !chB)|(chA & chB)) { encoder_3_position++; }
707	  else { encoder_3_position--; }
708	}
709	//Encoder on INT4
710	void encoder_4_change_m1() { //encoder4 mode 1x
711	  int chB=digitalRead(encoder_4_int2);
712	  if (!chB) { encoder_4_position++;}
713	  else { encoder_4_position--; }
714	}
715	void encoder_4_change_m2() { //encoder4 mode 2x
716	  int chB=digitalRead(encoder_4_int2);
717	  int chA=digitalRead(corresp[4]);
718	  if ((chA & !chB)|(!chA & chB)) { encoder_4_position++; }
719	  else { encoder_4_position--; }
720	}
721	void encoder_change_m4_A4(){//encoder4 mode 4x chA
722	  int chA=digitalRead(corresp[4]);
723	  int chB=digitalRead(encoder_4_int2);
724	  if ((chA & !chB)|(!chA & chB)) { encoder_4_position++; }
725	  else { encoder_4_position--; }
726	}
727	void encoder_change_m4_B4(){//encoder4 mode 4x chB
728	  int chA=digitalRead(corresp[4]);
729	  int chB=digitalRead(encoder_4_int2);
730	  if ((!chA & !chB)|(chA & chB)) { encoder_4_position++; }
731	  else { encoder_4_position--; }
732	}
733	//Encoder on INT5
734	void encoder_5_change_m1() { //encoder5 mode 1x
735	  int chB=digitalRead(encoder_5_int2);
736	  if (!chB) { encoder_5_position++;}
737	  else { encoder_5_position--; }
738	}
739	void encoder_5_change_m2() { //encoder5 mode 2x
740	  int chB=digitalRead(encoder_5_int2);
741	  int chA=digitalRead(corresp[5]);
742	  if ((chA & !chB)|(!chA & chB)) { encoder_5_position++; }
743	  else { encoder_5_position--; }
744	}
745	void encoder_change_m4_A5(){//encoder5 mode 4x chA
746	  int chA=digitalRead(corresp[5]);
747	  int chB=digitalRead(encoder_5_int2);
748	  if ((chA & !chB)|(!chA & chB)) { encoder_5_position++; }
749	  else { encoder_5_position--; }
750	}
751	void encoder_change_m4_B5(){//encoder5 mode 4x chB
752	  int chA=digitalRead(corresp[5]);
753	  int chB=digitalRead(encoder_5_int2);
754	  if ((!chA & !chB)|(chA & chB)) { encoder_5_position++; }
755	  else { encoder_5_position--; }
756	}
757	
758	/**************************************/
759	// Generic interrupt counter functions//
760	/**************************************/
761	//Counter on INT0
762	void counter_0_change() { //counter 0
763	  counter_0++;
764	}
765	//Counter on INT1
766	void counter_1_change() { //counter 1
767	  counter_1++;
768	}
769	//Counter on INT2
770	void counter_2_change() { //counter 2
771	  counter_2++;
772	}
773	//Counter on INT3
774	void counter_3_change() { //counter 3
775	  counter_3++;
776	}
777	//Counter on INT4
778	void counter_4_change() { //counter 4
779	  counter_4++;
780	}
781	//Counter on INT5
782	void counter_5_change() { //counter 5
783	  counter_5++;␊
784	}
785	

