// C++ code
//

#include <Arduino.h>


// attributes
int currentTram[5];
bool isGoodTram = true;

// definit si il peut etre touche par un tir aerien/sol ou les deux 0 ou 1
int whoCanTouchMe[1] = { 0 };
int value = 0;
bool isread = false;

enum State
{
    WAITING,
    START_TRAM,
    START_TRAM_D1,
    START_TRAM_D2,
    DATA_TRAM,
    DATA_TRAM_PARITY,
    WIN
};

const String strState[] = {"WAITING", "START_TRAM", "START_TRAM_D1", "START_TRAM_D2", "DATA_TRAM", "DATA_TRAM_PARITY", "WIN"};

String stateToString(int state)
{
    return strState[state];
}

// region Seuil

    int data;
    int seuil = 300;
    int tempsRead = 0;
    int tempsWrite = 0;
    int itSeuil = 0;
    float marge = 250;
    int seuilMed[20];
    int pinDetector = 35;

void calcSeuil(){
    seuil = 0;
    for (int i = 0; i < 20;i++)
    {
        seuil += seuilMed[i];
    }
    seuil = seuil/20;
}

int lastread = 0;

int getBit(int pin)
{
    lastread = analogRead(pin);
    if(lastread > seuil+marge) return 1;
    else return 0;
}

void secondLoop()
{

  if (millis() - tempsRead >= 250)
  {
    tempsRead = millis();
    int tmp = analogRead(pinDetector);
    if (tmp > seuil+marge)
    {
      seuilMed[itSeuil%20] = seuil+marge;
      
    }
    else if (tmp < seuil-marge)
    {
      seuilMed[itSeuil%20] = seuil-marge;
    }
    else
    {
      seuilMed[itSeuil%20] = tmp;
    }
    itSeuil++;
  }

  if(millis() - tempsWrite >= 1000)
  {
    tempsWrite = millis();
    calcSeuil();
  //  printf("\nSeuil Actuel : %d\n", seuil);
  }
}

// region Timer

class Timer
{
    unsigned long _elapsed = 0;
    unsigned long startTime = 0;

  	public:
    void start()
    {
        startTime = millis();
    }

    void restart()
    {
        startTime = millis();
    }

    unsigned long elapsed()
    {
        _elapsed = millis() - startTime;
        return _elapsed;
    }
};

Timer timer;

// region FSM

class FSM
{
    State _currentState = WAITING;

public:
    FSM();
    bool checkState(State src, State tgt, bool cond);
    State getCurrentState();
};

// implementation function FSM
FSM::FSM() : _currentState(WAITING)
{
}

bool FSM::checkState(State src, State tgt, bool cond = true)
{

    if (src == _currentState && cond)
    {
        _currentState = tgt;
    /*  Serial.println(strState[_currentState]
        +" |lastread:" + lastread 
        + " |Elapsed:" + timer.elapsed()
        + " |Value:" + value 
        + " |isGoodTram:"+ isGoodTram);*/  
        timer.restart();
        isread = false;
        return true;
    }
    return false;
}

State FSM::getCurrentState()
{
    return _currentState;
}

// endregion FSM

// declaration class
FSM fsm;



void runFSM()
{
    fsm.checkState(START_TRAM, WAITING, (!isGoodTram && timer.elapsed() >= 20) || timer.elapsed() >= 25);
    fsm.checkState(START_TRAM, START_TRAM_D1, currentTram[0] == 1 && timer.elapsed() >= 20);

    fsm.checkState(START_TRAM_D1, WAITING, (!isGoodTram && timer.elapsed() >= 20) || timer.elapsed() >= 25);
    fsm.checkState(START_TRAM_D1, START_TRAM_D2, currentTram[1] == 1 && timer.elapsed() >= 20);

    fsm.checkState(START_TRAM_D2, WAITING, (!isGoodTram && timer.elapsed() >= 20) || timer.elapsed() >= 25);
    fsm.checkState(START_TRAM_D2, DATA_TRAM, currentTram[2] == 0 && timer.elapsed() >= 20);

    fsm.checkState(DATA_TRAM, DATA_TRAM_PARITY, timer.elapsed() >= 20);

    fsm.checkState(WAITING, START_TRAM, value == 1);

    fsm.checkState(DATA_TRAM_PARITY, WAITING, (!isGoodTram && timer.elapsed() >= 20) || timer.elapsed() >= 25);

    fsm.checkState(DATA_TRAM_PARITY, WIN, isGoodTram && timer.elapsed() >= 20);
    
    
}

// region Lifecycle
void setup()
{
    Serial.begin(9600);
    timer.start();
    pinMode(pinDetector, INPUT);
    seuil = analogRead(pinDetector);
    for (int i = 0; i <20;i++) {
        seuilMed[i] = analogRead(pinDetector);
    }


}

void loop()
{
    value = getBit(pinDetector);
    secondLoop();
    runFSM();
    

    // run the current state
    switch(fsm.getCurrentState())
    {
        case WAITING: 
            // do nothing
            break;
        case START_TRAM:
            if (isread == true) break;
            
            if(value == 1 )
            {
                currentTram[0] = value;
            }
            else
            {
                isGoodTram = false;
            }
            isread = true;
            break;

        
        case START_TRAM_D1:
            if (isread == true) break;
            if(value == 1 )
            {
                currentTram[1] = value;
            }
            else
            {
                isGoodTram = false;
            }
            isread = true;
            break;

        case START_TRAM_D2:
            if (isread == true) break;
            if(value == 0 )
            {
                currentTram[2] = value;
            }
            else
            {
                isGoodTram = false;
            }
            isread = true;
            break;

        case DATA_TRAM: // check on the array pour l'instant c'est l un ou l'autre
            if (isread == true) break;
            if(value == whoCanTouchMe[0]) {
                currentTram[3]= value;
            } 
            else isGoodTram = false; 
            isread = true;
            break;

        case DATA_TRAM_PARITY:
            if (isread == true) break;
            isGoodTram == value != currentTram[3] ? true : false;
            isread = true;
            break;

        case WIN:
            Serial.print("\nYOUHOU\n");
            break;
    }
}
