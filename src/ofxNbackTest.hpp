//
//  ofxNbackTest.hpp
//  Soli20200921-Study01
//
//  Created by Ryo Hajika on 2020/09/23.
//

#pragma once

#include <vector>
#include <string>
#include <thread>
#include <chrono>

#include "ofLog.h"
#include "ofTrueTypeFont.h"
#include "ofGraphics.h"

#include "UtilityThings.h"

// similar impl as https://www.psytoolkit.org/experiment-library/nback.html
class ofxNbackTest {
public:
    ofxNbackTest();
    ~ofxNbackTest();
    
    void setup(unsigned n_val,
               unsigned interval_ms,
               unsigned blank_break_ms,
               float same_character_percentage,
               std::string font_path);
    void update();
    void draw(float center_x, float center_y, float width, float height);
    
    // study can be operated by start() and stop()
    void start();
    void stop();
    // getter func to check the study running state
    bool isTestRunning();
    bool isResponseSubmitted();
    
    void debugDraw(float x, float y){
        std::string s;
        s += "DISPLAY_MS: " + std::to_string(intvl_ms) + "\n";
        s += "RESPONSE_MS: " + std::to_string(break_ms) + "\n";
        s += "CURRENT_QUEUE: ";
        if(alphabets_buf.size()){
            for(int i=0; i<alphabets_buf.size(); i++){
                s += alphabets_buf[i];
                if(i != alphabets_buf.size() - 1){
                    s += " / ";
                }
            }
        }
        s += "\n";
        ofDrawBitmapStringHighlight(s, x, y);
    }
    
    bool submitResponse(bool yes_or_no);
    char getLastCharacter(){
        if(alphabets_buf.size()) return char(alphabets_buf.back()[0]);
    }
    int isLastResponseTrue(){
        return (int)resp_state;
    }
    
private:
    void renewCharacter();
    void displayCharacter(bool val);
    
    enum NBACK_RESPONSE_STATE {
        NBACK_RESPONSE_NONE = -999,
        NBACK_RESPONSE_WRONG = 0,
        NBACK_RESPONSE_CORRECT = 1
    };
    
    ofTrueTypeFont font;
    // as https://en.wikipedia.org/wiki/N-back
    std::string alphabets[8] = {"C", "H", "K", "L", "O", "Q", "R", "T"};
    const unsigned alphabets_size = 8;
    std::vector<std::string> alphabets_buf;
    unsigned n;
    unsigned intvl_ms;
    unsigned break_ms;
    float percentage;
    
    StopWatch sw;
    ofRectangle alphabet_bb;
    bool bCharacterDisplay;
    bool bIsRunning;
    bool bIsResponseSubmitted;
    NBACK_RESPONSE_STATE resp_state;
};
