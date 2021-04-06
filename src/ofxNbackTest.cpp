//
//  ofxNbackTest.cpp
//
//  Created by Ryo Hajika on 2020/09/23.
//

#include "ofxNbackTest.hpp"

ofxNbackTest::ofxNbackTest(){
}
ofxNbackTest::~ofxNbackTest(){
    this->cancelBGThread("intvl_thread");
    this->cancelBGThread("intvl_break_thread");
}

void ofxNbackTest::setup(unsigned n_val,
                         unsigned interval_ms,
                         unsigned blank_break_ms,
                         float same_character_percentage,
                         std::string font_path){ // 0 < percentage < 1
    _n = n_val;
    _intvl_ms = interval_ms;
    _break_ms = blank_break_ms;
    _percentage = same_character_percentage;
    _b_character_display = false;
    _b_running = false;
    _b_response_submitted = false;
    _resp_state = NBACK_RESPONSE_NONE;
    
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
    if(_b_running){
        ofSetColor(ofColor::white);
        if(_b_character_display){
            ofPushMatrix();
            ofTranslate(_alphabet_bb.width/-2., _alphabet_bb.height/2.);
            font.drawString(_alphabets_buf.back(), 0, 0);
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
        switch(_resp_state){
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
    _b_running = true;
    _b_response_submitted = false;
    if(_alphabets_buf.size()) _alphabets_buf.clear();
    this->renewCharacter();
}
void ofxNbackTest::stop(){
    this->cancelBGThread("intvl_thread");
    this->cancelBGThread("intvl_break_thread");
    _b_running = false;
}
// getter func to check the study running state
bool ofxNbackTest::isTestRunning(){
    return _b_running;
}
// always check this before you submit response to confirm the answering status
bool ofxNbackTest::isResponseSubmitted(){
    return _b_response_submitted;
}

// ofxNbackTest::submitResponse()
// returns you:
// TRUE -> if a response is accepted and the response is correct
// FALSE-> if a response is NOT accepted
//          OR you CANNOT answer at the point
//          OR the response is WRONG
bool ofxNbackTest::submitResponse(bool yes_or_no){
    if(_b_response_submitted){
        ofLogNotice("ofxNbackTest::submitResponse()", "response is already submitted!");
        return false;
    }else{
        double t = sw.lap();
        _b_response_submitted = true;
        
        if(_alphabets_buf.size() < _n+1){
            ofLogNotice("ofxNbackTest::submitResponse()", "result is not available at this point (Count < N)");
            _resp_state = NBACK_RESPONSE_NONE;
            return false;
        }else{
            bool b_fb_same = _alphabets_buf.front() == _alphabets_buf.back();
            if(yes_or_no == b_fb_same){
                ofLogNotice("ofxNbackTest::submitResponse()", "Correct! : %f", t);
                _resp_state = NBACK_RESPONSE_CORRECT;
                return true;
            }else{
                ofLogNotice("ofxNbackTest::submitResponse()", "Wrong! : %f", t);
                _resp_state = NBACK_RESPONSE_WRONG;
                return false;
            }
        }
    }
}

void ofxNbackTest::resizeSize(int n){
    this->stop();
    _n = n;
}
inline int ofxNbackTest::getSize(){
    return _n;
}

void ofxNbackTest::renewCharacter(){
    if(_alphabets_buf.size() < _n+1){
        _alphabets_buf.push_back(_alphabets[(unsigned)ofRandom(_alphabets_size)]);
    }else{
        float r = ofRandom(0., 1.);
        std::string f = _alphabets_buf.front();
        _alphabets_buf.erase(_alphabets_buf.begin());
        
        if(r < _percentage){
            _alphabets_buf.push_back(f);
        }else{
            _alphabets_buf.push_back(_alphabets[(unsigned)ofRandom(_alphabets_size)]);
        }
    }
    _alphabet_bb = font.getStringBoundingBox(_alphabets_buf.back(), 0, 0);
    this->displayCharacter(true);
    new_char_evt.notify(_alphabets_buf.back());
    
    if(_b_running){
        _resp_state = NBACK_RESPONSE_NONE;
        _b_response_submitted = false;
        
        _intvl_thread = std::thread([this](){
            std::this_thread::sleep_for(std::chrono::milliseconds(_intvl_ms));
            this->displayCharacter(false);
            bool _b = false;
            char_hidden_evt.notify(_b);
        });
        threadmap["intvl_thread"] = _intvl_thread.native_handle();
        _intvl_break_thread = std::thread([this](){
            std::this_thread::sleep_for(std::chrono::milliseconds(_intvl_ms+_break_ms));
            this->renewCharacter();
        });
        threadmap["intvl_break_thread"] = _intvl_break_thread.native_handle();
            
        _intvl_thread.detach();
        _intvl_break_thread.detach();
        
        sw.start();
    }
}
void ofxNbackTest::displayCharacter(bool val){
    _b_character_display = val;
}
void ofxNbackTest::cancelBGThread(const std::string & tname){
    ThreadMap::const_iterator it = threadmap.find(tname);
    if(it != threadmap.end()){
        pthread_cancel(it->second);
        threadmap.erase(tname);
    }
}
