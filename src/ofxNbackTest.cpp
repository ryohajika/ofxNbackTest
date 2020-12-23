//
//  ofxNbackTest.cpp
//  Soli20200921-Study01
//
//  Created by Ryo Hajika on 2020/09/23.
//

#include "ofxNbackTest.hpp"

ofxNbackTest::ofxNbackTest(){
    
}
ofxNbackTest::~ofxNbackTest(){
    
}

void ofxNbackTest::setup(unsigned n_val,
                         unsigned interval_ms,
                         unsigned blank_break_ms,
                         float same_character_percentage,
                         std::string font_path){ // 0 < percentage < 1
    n = n_val;
    intvl_ms = interval_ms;
    break_ms = blank_break_ms;
    percentage = same_character_percentage;
    bCharacterDisplay = false;
    bIsRunning = false;
    bIsResponseSubmitted = false;
    resp_state = NBACK_RESPONSE_NONE;
    
    font.load(font_path, 64, true, true, true);
}
void ofxNbackTest::update(){
    
}
void ofxNbackTest::draw(float center_x, float center_y, float width, float height){
    ofPushMatrix();
    ofPushStyle();
    // adjust coordinates to draw everythng based on a center position
    ofTranslate(center_x, center_y);
    // draw background
    ofSetColor(ofColor::black);
    ofPushMatrix();
    ofTranslate(width/-2., height/-2.);
    ofDrawRectangle(0, 0, width, height);
    ofPopMatrix();
    
    // draw character and marker
    if(bIsRunning){
        ofSetColor(ofColor::white);
        if(bCharacterDisplay){
            ofPushMatrix();
            ofTranslate(alphabet_bb.width/-2., alphabet_bb.height/2.);
            font.drawString(alphabets_buf.back(), 0, 0);
            ofPopMatrix();
        }else{
            ofPushMatrix();
            ofTranslate(-25., -25.);
            ofDrawRectangle(0, 20, 50, 10);
            ofDrawRectangle(20, 0, 10, 50);
            ofPopMatrix();
        }
        
        ofPushMatrix();
        ofTranslate(-30, 40);
        switch(resp_state){
            case NBACK_RESPONSE_NONE:
                ofSetColor(ofColor::darkGray);
                break;
            case NBACK_RESPONSE_CORRECT:
                ofSetColor(ofColor::green);
                break;
            case NBACK_RESPONSE_WRONG:
                ofSetColor(ofColor::red);
                break;
        }
        ofDrawRectangle(0, 0, 60, 10);
        ofPopMatrix();
    }
    ofPopStyle();
    ofPopMatrix();
}

// study can be operated by start() and stop()
void ofxNbackTest::start(){
    bIsRunning = true;
    bIsResponseSubmitted = false;
    if(alphabets_buf.size()) alphabets_buf.clear();
    this->renewCharacter();
}
void ofxNbackTest::stop(){
    bIsRunning = false;
}
// getter func to check the study running state
bool ofxNbackTest::isTestRunning(){
    return bIsRunning;
}
// always check this before you submit response to confirm the answering status
bool ofxNbackTest::isResponseSubmitted(){
    return bIsResponseSubmitted;
}

// ofxNbackTest::submitResponse()
// returns you:
// TRUE -> if a response is accepted and the response is correct
// FALSE-> if a response is NOT accepted
//          OR you CANNOT answer at the point
//          OR the response is WRONG
bool ofxNbackTest::submitResponse(bool yes_or_no){
    if(bIsResponseSubmitted){
        ofLogNotice("ofxNbackTest::submitResponse()", "response is already submitted!");
        return false;
    }else{
        double t = sw.lap();
        bIsResponseSubmitted = true;
        
        if(alphabets_buf.size() < n+1){
            ofLogNotice("ofxNbackTest::submitResponse()", "result is not available at this point (Count < N)");
            resp_state = NBACK_RESPONSE_NONE;
            return false;
        }else{
            bool b_fb_same = alphabets_buf.front() == alphabets_buf.back();
            if(yes_or_no == b_fb_same){
                ofLogNotice("ofxNbackTest::submitResponse()", "Correct! : %f", t);
                resp_state = NBACK_RESPONSE_CORRECT;
                return true;
            }else{
                ofLogNotice("ofxNbackTest::submitResponse()", "Wrong! : %f", t);
                resp_state = NBACK_RESPONSE_WRONG;
                return false;
            }
        }
    }
}

void ofxNbackTest::renewCharacter(){
    if(alphabets_buf.size() < n+1){
        alphabets_buf.push_back(alphabets[(unsigned)ofRandom(alphabets_size)]);
    }else{
        float r = ofRandom(0., 1.);
        std::string f = alphabets_buf.front();
        alphabets_buf.erase(alphabets_buf.begin());
        
        if(r < percentage){
            alphabets_buf.push_back(f);
        }else{
            alphabets_buf.push_back(alphabets[(unsigned)ofRandom(alphabets_size)]);
        }
    }
    alphabet_bb = font.getStringBoundingBox(alphabets_buf.back(), 0, 0);
    this->displayCharacter(true);
    
    if(bIsRunning){
        resp_state = NBACK_RESPONSE_NONE;
        bIsResponseSubmitted = false;
        
        std::thread([this](){
            std::this_thread::sleep_for(std::chrono::milliseconds(intvl_ms));
            this->displayCharacter(false);
        }).detach();
        std::thread([this](){
            std::this_thread::sleep_for(std::chrono::milliseconds(intvl_ms+break_ms));
            this->renewCharacter();
        }).detach();
        
        sw.start();
    }
}
void ofxNbackTest::displayCharacter(bool val){
    bCharacterDisplay = val;
}
