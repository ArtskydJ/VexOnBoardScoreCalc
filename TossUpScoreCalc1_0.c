/*						 +------------------------------+
						 |   Toss Up Score Calculator	|
						 |     Joseph D.  Team 3018		|
						 |   2013-04-30    2013-05-01	|
+------------------------+------------------------------+
|     In this code is a trick to use more than just 4	|
| timers in robotC. You just declare some unsigned long	|
| variables (like "timeStart" or "timeRefresh") and add	|
| the delta time to them every time the code loops.	You	|
| can have as many of these as you want. They are quite |
| accurate, but probably not perfect. RobotC should run	|
| a heavy loop in under 6 MS, so that is the max error	|
| that the Fake Timer will have. The reason that the	|
| delta time is added, is so that you can reset the		|
| fake timer by setting it to 0. E.g. timeStart=0;		|
|     BTW, the fake timers could backfire if your code	|
| loops in under 1 MS. The delta time would be 0, which	|
| would add nothing to the fake timers and then reset	|
| the real timer. To fix this, a minimum loop time has	|
| been implemented. Also, minimum loop time keeps PID	|
| controllers more accurate (because I and D need time	|
| consistency or the deltatime multiplied in).			|
|     Feel free to use these tricks in your code. If	|
| you want to give credit, that's great, but it's not	|
| required.												|
+-------------------------------------------------------+  */
#define NOMAIN   2 //NO means Number Of
#define NOMID    3
#define NOBUCKY   3
#define NOLARGE    4
#define NOROBOT     3
#define NOBUCKYOBJS  10
#define NOLARGEOBJS  4
#define BTNNEXTTIME  650  //If you hold a button to go to the next item it will auto-repeat after this many MS
#define BTNLOOPTIME  2000 //If you hold a button to loop through the menu again it will auto-repeat after this many MS
#define BTNADDTIME  350   //If you hold a button to add or subtract the current value it will auto-repeat after this many MS
#define MINLOOPTIME  2    //For full vex code, this might be 3 or 4.
#define MODEINPUT    0
#define MODEDISPLAY  1
#define MODERESTART  2


bool btnLeft=0;
bool btnCenter=0;
bool btnRight=0;
bool lastBtnLeft=0;
bool lastBtnCenter=0;
bool lastBtnRight=0;
int NOobjectsScored[NOMAIN*NOMID][4]; //See notes below
//4 is the largest of NOBUCKY, NOLARGE and NOROBOT
//This array corresponds with pointValues[][] below.
//Also, I had to use NOmain and NOmid in the same index because robotc does not allow more than 2 indices.
char menuMainItem=0;
char menuMidItem=0;
char menuBuckyItem=0;
char menuLargeItem=0;
char menuRobotItem=0;
char mode=MODEINPUT;
string line1="";
string line2="";
string lastLine1="";
string lastLine2="";
unsigned long timeBtn=0;
unsigned long timeT1;
const string menuMain[NOMAIN]=		{"Red","Blue"};										//COLOR
const string menuMid[NOMID]=		{"Bucky Balls","Large Balls","Robots"};				//TYPE
const string menuBucky[NOBUCKY]=	{"Middle Zone","Goal Zone","Stashed"};				//Bucky
const string menuLarge[NOLARGE]=	{"Middle Zone","Goal Zone","Stashed","Hanging"};	//Large
const string menuRobot[NOROBOT]=	{"Low Hanging","High Hanging","Auton Bonus"};		//Robot
const char pointValues[3][4]={
{1, 2,  5,  0},		//Bucky
{1, 5,  10, 10},	//Large
{5, 10, 10, 0}};	//Robot

/*
	The Function init() initializes and resets.
*/
void init()
	{
	ClearTimer(T1);
	menuMainItem=0;
	menuMidItem=0;
	menuBuckyItem=0;
	menuLargeItem=0;
	menuRobotItem=0;
	mode=MODEINPUT;
	timeBtn=0;
	}


/*
	The Function input() gathers all sensor values
values and stores them in variables for later use.
This is done so that there are no conflicting sensor
values throughout the code since all sensors are read
once, and that reading is always used.
*/
void input()
	{
	btnLeft   = (bool)(nLCDButtons&1);
	btnCenter = (bool)(nLCDButtons&2);
	btnRight  = (bool)(nLCDButtons&4);

	timeT1=time1[T1];
	ClearTimer(T1);
	//Fake Timers Here
	timeBtn+=timeT1;
	if (!btnLeft && !btnCenter && !btnRight && timeBtn<BTNNEXTTIME) timeBtn=BTNNEXTTIME; //See notes below
	//If all buttons are released than set timer forward so that no delay is implemented.
	//This will not work for the emulator, because it has auto repeat on LCD buttons.
	}

/*
	The Function process() precesses the inputs
and sets variables for outputs.
*/
void process()
	{
	if (mode==MODEINPUT)
		{
		if ( timeBtn>=BTNADDTIME && (btnLeft || btnRight) ) //CHANGE VALUE OF CURRENT ITEM
			{
			timeBtn=0;
			int tSelectedNum, tMax, tItem; //a prefix of 't' means it is a temporary variable.
			switch (menuMidItem)
				{
				case 0: {tItem=menuBuckyItem; tMax=NOBUCKYOBJS;} break;
				case 1: {tItem=menuLargeItem; tMax=NOLARGEOBJS;} break;
				case 2: {tItem=menuRobotItem; tMax=2;} break;
				}
			tSelectedNum = NOobjectsScored[menuMainItem*NOMID+menuMidItem][tItem];
			if (btnLeft) //SUBTRACT FROM CURRENT ITEM
				{
				tSelectedNum--;
				if (tSelectedNum<0) tSelectedNum=0;
				}
			if (btnRight) //ADD TO CURRENT ITEM
				{
				tSelectedNum++;
				if (tSelectedNum>tMax) tSelectedNum=tMax;
				}
			NOobjectsScored[menuMainItem*NOMID+menuMidItem][tItem] = tSelectedNum;
			}
		if (timeBtn>=BTNNEXTTIME && btnCenter) //GO TO NEXT ITEM
			{
			timeBtn=0;
			switch (menuMidItem)
				{
				case 0: {menuBuckyItem++; if (menuBuckyItem>=NOBUCKY) {menuBuckyItem=0; menuMidItem++;}} break;
				case 1: {menuLargeItem++; if (menuLargeItem>=NOLARGE) {menuLargeItem=0; menuMidItem++;}} break;
				case 2: {menuRobotItem++; if (menuRobotItem>=NOROBOT) {menuRobotItem=0; menuMidItem++;}} break;
				}
			if (menuMidItem>=NOMID) {menuMidItem=0; menuMainItem++;}
			if (menuMainItem>=NOMAIN) {menuMainItem=0; mode=MODEDISPLAY;}
			}
		}
	else if (mode==MODEDISPLAY)
		{
		if (timeBtn>=BTNLOOPTIME && btnCenter) //GO TO NEXT ITEM
			{
			timeBtn=0;
			mode=MODERESTART;
			}
		}
	}

/*
	The Function processScreen() takes inputted and
processed variables and sets screen variables for
output.
*/
void processScreen()
	{
	string tempString="";
	if (mode==MODEINPUT) //If still setting numbers
		{
		/*
		Set line 1 to the Color  and Type.
		- Color: Red,Blue
		- Type: Bucky Ball, Large Ball, Robot Actions
		Set line 2 to the Position and Number.
		- Position: Middle Zone, Goal Zone, Stashed, Low Hanging, High Hanging, Auton Bonus
		- Number: How many Types you have in the respective Positions
		*/
		sprintf(line1,"%s %s",menuMain[menuMainItem],menuMid[menuMidItem]);
		int tempItem;
		switch (menuMidItem)
			{
			case 0: {tempString=menuBucky[menuBuckyItem]; tempItem=menuBuckyItem;} break;
			case 1: {tempString=menuLarge[menuLargeItem]; tempItem=menuLargeItem;} break;
			case 2: {tempString=menuRobot[menuRobotItem]; tempItem=menuRobotItem;} break;
			}
		sprintf(line2,"%s:%02d",tempString,NOobjectsScored[menuMainItem*NOMID+menuMidItem][tempItem]);
		}
	else if (mode==MODEDISPLAY)//If viewing score
		{
		/*
		Set line 1 to how many points Red and Blue have individually.
		Set line 2 to who won and by how many points.
		*/
		int tempPoints[2]={0,0};
		for (int j=0; j<2; j++)
			{
			for (int k=0; k<3; k++)
				{
				for (int l=0; l<4; l++)
					{
					tempPoints[j]+=(NOobjectsScored[j*NOMID+k][l])*(pointValues[k][l]);
					}
				}
			}
		sprintf(line1,"Red:%1d Blue:%1d",tempPoints[0],tempPoints[1]);
		if (tempPoints[1]<tempPoints[0])  sprintf(line2,"Red wins by %d",(tempPoints[0]-tempPoints[1]));
		if (tempPoints[0]<tempPoints[1])  sprintf(line2,"Blue wins by %d",(tempPoints[1]-tempPoints[0]));
		if (tempPoints[1]==tempPoints[0]) sprintf(line2,"Red ties Blue!");
		}
	}
/*
	The Function output() takes all previous set
variables and assigns them to the outputs.
*/
void output()
	{
	if (line1!=lastLine1) {clearLCDLine(0); displayLCDCenteredString(0,line1);}
	if (line2!=lastLine2) {clearLCDLine(1); displayLCDCenteredString(1,line2);}
	while (time1(T2)<MINLOOPTIME){}
	ClearTimer(T2);
	}

/*
	The Function setLasts() has all "last" variables
set at the same time and in one place. If code acts
funny like toggling values really fast when pressing a
button or only recognizing one press it might be due
to not having the "last" value set. Having them all
set in one place makes it easy to see what is being
set instead of looking all over your code.
*/
void setLasts()
	{
	lastBtnLeft   = btnLeft;
	lastBtnCenter = btnCenter;
	lastBtnRight  = btnRight;
	lastLine1 = line1;
	lastLine2 = line2;
	}

/*
The Function main() runs all of the code. It is
short and concise due to the code being split up into
functions.
Main() runs and infinite loop
*/
task main()
	{
	while(1)
		{
		init();
		while(mode!=MODERESTART)
			{
			input();
			process();
			processScreen();
			output();
			setLasts();
			}
		}
	}
