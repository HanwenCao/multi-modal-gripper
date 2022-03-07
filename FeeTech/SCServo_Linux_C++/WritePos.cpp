/*
舵机出厂速度单位为0.732rpm，舵机机运行速度V=80
舵机出厂速度单位是0.0146rpm，速度改为V=80*50
*/

#include <iostream>
#include "SCServo.h"

SMS_STS sm_st;

int main(int argc, char **argv)
{
	if(argc<2){
        std::cout<<"argc error!"<<std::endl;
        return 0;
	}
	std::cout<<"serial:"<<argv[1]<<std::endl;
    if(!sm_st.begin(115200, argv[1])){
        std::cout<<"Failed to init sms/sts motor!"<<std::endl;
        return 0;
    }
	int Pos;
	int Speed;
	int Load;
	int Voltage;
	int Temper;
	int Move;
	int Current;
	int goal = 4095;
	int threshold = 20;
	int index = 0;
	int speed = 1000;
	int acc = 1000;
	int overload_thresh = 400;
	int overload_flag = 0;
	
	while(1){
	
		//Change goal position
		if(goal == 4095){goal = 0;}
		else{goal = 4095;}
		
		while(1){ 
		    //read (state update)
		    if(sm_st.FeedBack(1)!=-1){
			    Pos = sm_st.ReadPos(-1);//-1表示缓冲区数据，以下相同
			    //Speed = sm_st.ReadSpeed(-1);
			    Load = sm_st.ReadLoad(-1);
			    //Voltage = sm_st.ReadVoltage(-1);
			    //Temper = sm_st.ReadTemper(-1);
			    //Move = sm_st.ReadMove(-1);
			    //Current = sm_st.ReadCurrent(-1);
			    //std::cout<<"pos = "<<Pos<<" ";
			    //std::cout<<"Speed = "<<Speed<<" ";
			    std::cout<<"Load = "<<Load<<" ";
			    //std::cout<<"Voltage = "<<Voltage<<" ";
			    //std::cout<<"Temper = "<<Temper<<" ";
			    //std::cout<<"Move = "<<Move<<" ";
			    //std::cout<<"Current = "<<Current;
			    std::cout<<std::endl;
			    usleep(1*1000);
		    }else{
			    std::cout<<"read err"<<std::endl;
			    sleep(1);
		    }
		    
		    // set goal
		    if(overload_flag == 0){
		        //std::cout<<"set goal = "<<goal<<std::endl;
		        sm_st.WritePosEx(1, goal, speed, acc);//move
		    }
		    else{
		        //std::cout<<"set goal = "<<Pos<<std::endl;
		        sm_st.WritePosEx(1, Pos, speed, acc);//dont move
		    }
		    
		    // check if arrived
            if(abs(Pos-goal) < threshold){
                break;
            }
            
            // check if loaded (contact detection)
            if(abs(Load) > overload_thresh){
                std::cout<<"over load"<<std::endl;
                overload_flag = 1;
                break;
            }
		}
	}
	sm_st.end();
	return 1;
}

