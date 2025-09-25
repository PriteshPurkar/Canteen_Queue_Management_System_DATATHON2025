Problem Statement :  Long queues in the canteen lead to delays during lunch breaks. Design a system that counts people in the queue , displays waiting times, and sends alerts to students when their turn is near.

In this project the Key Challenges were :
1) Queue Over Crowding :- Unorganized queue formation leading to inefficiency.
2) Poor Communication :- Students unaware of queue status and order readiness.
3) Time Estimation Issues :- No accurate waiting time prediction affects students' schedules.

So basically what we have observed that the Lunch break timings of all the students is same and there are a specific servers(2-3) people serving thali at a time creating a long queue and ding changes in time isn't just the Solution. So by getting inspired by the RFID system at Decathlon Showrooms we thought that already a student has a normal ID card, why not RFID?? if it becomes RFID it becomes easy to track and execute.

Solution : So firstly what do we need? a) Proper Interaction system -- At the entry of the canteen and At the buffet counter while collecting the thalis. (ESP8266 + RFID reader)
                Secondly we need a proper Database so we are using MongoDB here. 
                Thirdly we need is a proper Backend to properly function between the hardware and the user. (Python -- Flask)
                Fourthly we need a proper communication System So that Overcrowding can be dealt. ( Twilio API's)

Process Flow : 

<img width="1361" height="321" alt="Screenshot 2025-09-24 025900" src="https://github.com/user-attachments/assets/b0f1cfe5-f2d7-4267-8285-3010cc1927bc" />


Architecture Diagram : 


<img width="842" height="468" alt="image" src="https://github.com/user-attachments/assets/1a666d2b-ac24-4def-860f-dda40bde61f4" />




Presentation Video : 
https://drive.google.com/file/d/1X__kK0x_B8We0myADOVEGiXUDb9Lwxco/view?usp=drive_link


For Idea Understanding jump directly on : 2:50 


Won 3rd Prize in Competition : 

<img width="1296" height="919" alt="Screenshot 2025-09-25 172948" src="https://github.com/user-attachments/assets/210e8e5b-6319-485e-84ab-f59fbd86dec5" />



