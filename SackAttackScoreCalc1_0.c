/*						 +------------------------------+
						 | Sack Attack Score Calculator	|
						 |     Joseph D.  Team 3018		|
						 |   2013-05-09    2013-05-09	|
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
#define NO_COLOR   2 //NO means Number Of
#define NO_OBJECT    3
#define NO_SACKS   3
#define NO_BONUS    3
#define NO_ROBOT     2
#define NO_SACKSOBJS  100
#define NO_BONUSOBJS  4
#define NO_ROBOTOBJS  2
#define BTNNEXTTIME  650  //If you hold a button to go to the next item it will auto-repeat after this many MS
#define BTNLOOPTIME  2000 //If you hold a button to loop through the menu again it will auto-repeat after this many MS
#define BTNADDTIME   350  //If you hold a button to add or subtract the current value it will auto-repeat after this many MS
#define MINLOOPTIME  2    //For full vex code, this might be 3 or 4.
#define MODE_INPUT    0
#define MODE_DISPLAY  1
#define MODE_RESTART  2


bool btnLeft=0;
bool btnCenter=0;
bool btnRight=0;
bool lastBtnLeft=0;
bool lastBtnCenter=0;
bool lastBtnRight=0;
int NOobjectsScored[NO_COLOR*NO_OBJECT][3]; //See notes below
//4 is the largest of NO_SACKS, NO_BONUS and NO_ROBOT
//This array corresponds with pointValues[][] below.
//Also, I had to use NO_COLOR and NO_OBJECT in the same index because robotc does not allow more than 2 indices.
char menuColorItem=0;
char menuObjectItem=0;
char menuSackItem=0;
char menuBonusItem=0;
char menuRobotItem=0;
char mode=MODE_INPUT;
string line1="";
string line2="";
string lastLine1="";
string lastLine2="";
unsigned long timeBtn=0;
unsigned long timeT1;
const string menuColor[NO_COLOR] =		{"Red","Blue"};							//COLOR
const string menuObject[NO_OBJECT] =	{"Sacks","Bonus sacks","Alliance"};		//OBJECT
const string menuSackPos[NO_SACKS] =	{"Starting tile","Trough","High goal"};	//Bucky
const string menuBonusPos[NO_BONUS] =	{"Starting tile","Trough","High goal"};	//Large
const string menuRobotPos[NO_ROBOT] =	{"Parking Bonus","Auton Bonus"};		//Robot
const char pointValues[3][3]={
{1,	5,	10},	//Sacks
{6,	10,	15},	//Bonus
{5,	10,	10}};	//Robot

/*
	The Function init() initializes and resets.
*/
void init()
	{
	ClearTimer(T1);
	menuColorItem=0;
	menuObjectItem=0;
	menuSackItem=0;
	menuBonusItem=0;
	menuRobotItem=0;
	mode=MODE_INPUT;
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
	if (mode==MODE_INPUT)
		{
		if ( timeBtn>=BTNADDTIME && (btnLeft || btnRight) ) //CHANGE VALUE OF CURRENT ITEM
			{
			timeBtn=0;
			int tSelectedNum, tMax, tItem; //a prefix of 't' means it is a temporary variable.
			switch (menuObjectItem)
				{
				case 0: {tItem=menuSackItem; tMax=NO_SACKSOBJS;} break;
				case 1: {tItem=menuBonusItem; tMax=NO_BONUSOBJS;} break;
				case 2: {tItem=menuRobotItem; tMax=NO_ROBOTOBJS;} break;
				}
			tSelectedNum = NOobjectsScored[menuColorItem*NO_OBJECT+menuObjectItem][tItem];
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
			NOobjectsScored[menuColorItem*NO_OBJECT+menuObjectItem][tItem] = tSelectedNum;
			}
		if (timeBtn>=BTNNEXTTIME && btnCenter) //GO TO NEXT ITEM
			{
			timeBtn=0;
			switch (menuObjectItem)
				{
				case 0: {menuSackItem++; if (menuSackItem>=NO_SACKS) {menuSackItem=0; menuObjectItem++;}} break;
				case 1: {menuBonusItem++; if (menuBonusItem>=NO_BONUS) {menuBonusItem=0; menuObjectItem++;}} break;
				case 2: {menuRobotItem++; if (menuRobotItem>=NO_ROBOT) {menuRobotItem=0; menuObjectItem++;}} break;
				}
			if (menuObjectItem>=NO_OBJECT) {menuObjectItem=0; menuColorItem++;}
			if (menuColorItem>=NO_COLOR) {menuColorItem=0; mode=MODE_DISPLAY;}
			}
		}
	else if (mode==MODE_DISPLAY)
		{
		if (timeBtn>=BTNLOOPTIME && btnCenter) //GO TO NEXT ITEM
			{
			timeBtn=0;
			mode=MODE_RESTART;
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
	if (mode==MODE_INPUT) //If still setting numbers
		{
		/*
		Set line 1 to the Color  and Type.
		- Color: Red,Blue
		- Type: Sack, Bonus, Robot
		Set line 2 to the Position and Number.
		- Position: Middle Zone, Goal Zone, Stashed, Low Hanging, High Hanging, Auton Bonus
		- Number: How many Types you have in the respective Positions
		*/
		sprintf(line1,"%s|%s",menuColor[menuColorItem],menuObject[menuObjectItem]);
		int tempItem;
		switch (menuObjectItem)
			{
			case 0: {tempString=menuSackPos[menuSackItem];   tempItem=menuSackItem;} break;
			case 1: {tempString=menuBonusPos[menuBonusItem]; tempItem=menuBonusItem;} break;
			case 2: {tempString=menuRobotPos[menuRobotItem]; tempItem=menuRobotItem;} break;
			}
		sprintf(line2,"%s:%1d",tempString,NOobjectsScored[menuColorItem*NO_OBJECT+menuObjectItem][tempItem]);
		}
	else if (mode==MODE_DISPLAY)//If viewing score
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
				for (int l=0; l<3; l++)
					{
					tempPoints[j]+=(NOobjectsScored[j*NO_OBJECT+k][l])*(pointValues[k][l]);
					}
				}
			}
		sprintf(line1,"Red:%1d|Blue:%1d",tempPoints[0],tempPoints[1]);
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
		while(mode!=MODE_RESTART)
			{
			input();
			process();
			processScreen();
			output();
			setLasts();
			}
		}
	}
