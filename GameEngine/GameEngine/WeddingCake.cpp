//
//  WeddingCake.cpp
//  Project3_wedding_cake
//
//  Created by Chenxing Ouyang on 5/5/16.
//  Copyright © 2016 Chenxing Ouyang. All rights reserved.
//
#include "OBJObject.h"
#include "Node.h"

#include "WeddingCake.h"

// This is used to scale up the pingpong range so we can use integers for each step
float pingpong_step_factor = 100;

// This is the arm radius of the pod propeller
float pod_propeller_arm_radius = 0.3f;
// This is the arm length of the pod propeller
float pod_propeller_arm_length = 1;
// This handles the pingpong value of the pod propeller's expandable arms
int min_pods = pod_propeller_arm_length * pingpong_step_factor * 0.5f;
int max_pods = pod_propeller_arm_length * pingpong_step_factor * 1.5f;
int step_pods = 10;
int d_pods = max_pods - min_pods;
int n_pods = d_pods;
int pingpong_value_pods;
float normalize_factor_pods = 100.0f;

// This is the arm radius of the center post's arms
float center_post_arm_radius = 0.5f;
// This is the arm length of that hold each pod propeller, it is attached to the center post.
float center_post_propeller_arm_length = 4;
// This handles the pingpong value of the center post's expandable arms
int min_center = center_post_propeller_arm_length * pingpong_step_factor * 0.5f;
int max_center = center_post_propeller_arm_length * pingpong_step_factor * 1.5f;
int step_center = 10;
int d_center = max_center - min_center;
int n_center = d_center;
int pingpong_value_center;
float normalize_factor_center = 100.0f;

// This determins how fast each propeller (3 pods) spins
float propeller_spin_angle = 0;
float propeller_spin_speed = 4.0f;

// This determins how fast each level spins
float center_post_spin_angle = 0;
float center_post_spin_speed = 2.0f;

// This determins how fast each pod spins
float pods_spin_angle = 0;
float pods_spin_speed = 5.0f;

// Move the pod above the arm that holds it so that it won't touch the arm when spinning
float pods_propeller_elevation = 0.5f;
// This determisn the offset between each level
float level_offset = 5.0f;
// This is the height of the center post
float center_post_height = 12.0f;
// This is the dimension of the base
float base_dim = 20.0f;

int num_of_pods = 27;
int num_of_pods_arms = 54;
int num_of_center_post_arms = 18;

Geode * centerPost = nullptr;
Geode * centerPostPingPong = nullptr; // Used to make the main vertical post extend and contract
Geode * base = nullptr;
std::vector<Geode*> pods; // creates 27 pods for 9 propellers
std::vector<Geode*> pods_arms; // creates 54 extendable propeller arms for 9 propellers's pods
std::vector<Geode*> center_post_arms; // creates 18 extendable propeller arms for the center post

WeddingCake::WeddingCake(GLuint shaderProgram, OBJObject * cylinder, OBJObject * pod, OBJObject * bearObj)
{
    this->weddingCake = nullptr;
    this->bearObj = bearObj;
    
    centerPost = new Geode(cylinder, shaderProgram);
    // Stretch the center post to be longer
    centerPost->M = glm::translate(glm::mat4(1), glm::vec3(0, -center_post_height/2.0f, 0)) * glm::scale(glm::mat4(1), glm::vec3(1, center_post_height, 1));
    
    centerPostPingPong = new Geode(cylinder, shaderProgram);

    base = new Geode(cylinder, shaderProgram);
    // Scale bigger then put it below the post
    base->M = glm::translate(glm::mat4(1), glm::vec3(0, -center_post_height, 0)) * glm::scale(glm::mat4(1), glm::vec3(base_dim, 1, base_dim));
    
    for(int i = 0; i < num_of_pods; i++) pods.push_back(new Geode(pod, shaderProgram));
    for(int i = 0; i < num_of_pods_arms; i++) pods_arms.push_back(new Geode(cylinder, shaderProgram));
    for(int i = 0; i < num_of_center_post_arms; i++) center_post_arms.push_back(new Geode(cylinder, shaderProgram));
}

WeddingCake::~WeddingCake()
{
    delete centerPost;
    delete centerPostPingPong;
    delete base;
    for(int i = 0; i < num_of_pods; i++) delete pods[i];
    for(int i = 0; i < num_of_pods_arms; i++) delete pods_arms[i];
    for(int i = 0; i < num_of_center_post_arms; i++) delete center_post_arms[i];
}

void WeddingCake::addBearToPod(int podIndex)
{
    // since there are 27 pods you can only choose a pod from 0 to 26
    if (podIndex >= 0 && podIndex <= 26)
    {
        glm::mat4 bearLocalTransformToPod = glm::translate(glm::mat4(1), glm::vec3(0,0.3,0));
        pods[podIndex]->addBear(bearObj, bearLocalTransformToPod);
    }
}

void WeddingCake::preUpdate()
{
    // Update the pinpong value for the pods extendable arms
    pingpong_value_pods = min_pods + abs(d_pods - n_pods); // the result
    n_pods = (n_pods + step_pods) % (2 * d_pods); // the step
    
    
    // Update the pinpong value for the center post extendable arms
    pingpong_value_center = min_center + abs(d_center - n_center); // the result
    n_center = (n_center + step_center) % (2 * d_center); // the step
    
    
    center_post_spin_angle += center_post_spin_speed;
    if (center_post_spin_angle > 360.0f || center_post_spin_angle < -360.0f)
        center_post_spin_angle = 0.0f;
    
    propeller_spin_angle += propeller_spin_speed;
    if (propeller_spin_angle > 360.0f || propeller_spin_angle < -360.0f)
        propeller_spin_angle = 0.0f;
    
    pods_spin_angle += pods_spin_speed;
    if (pods_spin_angle > 360.0f || pods_spin_angle < -360.0f)
        pods_spin_angle = 0.0f;
}

// Returns a propeller with extendable 3 pods
MatrixTransform * WeddingCake::createPropeller(int propeller_index)
{
    int pods_arm_index = propeller_index * 6;
    int pods_index = propeller_index * 3;
    
    auto pod1 = pods[0 + pods_index];
    auto pod2 = pods[1 + pods_index];
    auto pod3 = pods[2 + pods_index];
    
    auto arm1 = pods_arms[0 + pods_arm_index];
    auto arm2 = pods_arms[1 + pods_arm_index];
    auto arm3 = pods_arms[2 + pods_arm_index];
    auto arm4 = pods_arms[3 + pods_arm_index];
    auto arm5 = pods_arms[4 + pods_arm_index];
    auto arm6 = pods_arms[5 + pods_arm_index];
    
    
    glm::mat4 spin_mat = glm::rotate(glm::mat4(1.0f), propeller_spin_angle / 180.0f * glm::pi<float>(), glm::vec3(0, 1, 0));
    glm::mat4 spin_mat2 = glm::rotate(glm::mat4(1.0f), pods_spin_angle / 180.0f * glm::pi<float>(), glm::vec3(1, 0, 0));
    glm::mat4 r = glm::rotate(glm::mat4(1.0), 90.0f / 180.0f * glm::pi<float>(), glm::vec3(0, 0, 1));
    glm::mat4 s1 = glm::scale(glm::mat4(1.0), glm::vec3(pod_propeller_arm_length, pod_propeller_arm_radius, pod_propeller_arm_radius));
    glm::mat4 t1 = glm::translate(glm::mat4(1.0), glm::vec3(pod_propeller_arm_length/2.0f,0,0));
    glm::mat4 t2 = glm::translate(glm::mat4(1.0), glm::vec3(pingpong_value_pods/normalize_factor_pods,0,0));
    // Sicnce the arms that holds the pods and the pods are extendable, use pingpong value to make them strech in and out
    glm::mat4 t3 = glm::translate(glm::mat4(1.0), glm::vec3(pod_propeller_arm_length/2.0f + pingpong_value_pods/normalize_factor_pods + 0.3f,0,0));
    
    MatrixTransform * spinning_propeller = new MatrixTransform(glm::mat4(1.0));
    
    // R1 R2 R3 creates a stable non spinning propeller
    MatrixTransform * R1 = new MatrixTransform(glm::rotate(glm::mat4(1.0), 0.0f / 180.0f * glm::pi<float>(), glm::vec3(0, 0, 1)));
    MatrixTransform * R2 = new MatrixTransform(glm::rotate(glm::mat4(1.0), 120.0f / 180.0f * glm::pi<float>(), glm::vec3(0, 1, 0)));
    MatrixTransform * R3 = new MatrixTransform(glm::rotate(glm::mat4(1.0), 240.0f / 180.0f * glm::pi<float>(), glm::vec3(0, 1, 0)));
    
    // Now creates the spinning matrix that spins the level
    MatrixTransform * S = new MatrixTransform(spin_mat);
    
    arm1->M = t1 * s1 * r;
    arm2->M = t1 * s1 * r;
    arm3->M = t1 * s1 * r;
    
    arm4->M = t2 * s1 * r;
    arm5->M = t2 * s1 * r;
    arm6->M = t2 * s1 * r;
    
    pod1->M = t3 * spin_mat2;
    pod2->M = t3 * spin_mat2;
    pod3->M = t3 * spin_mat2;
    
    spinning_propeller->addChild(S);
    S->addChild(R1);
    S->addChild(R2);
    S->addChild(R3);
    R1->addChild(arm1);
    R2->addChild(arm2);
    R3->addChild(arm3);
    R1->addChild(arm4);
    R2->addChild(arm5);
    R3->addChild(arm6);
    
    R1->addChild(pod1);
    R2->addChild(pod2);
    R3->addChild(pod3);
    
    
    return spinning_propeller;
}


// Returning a spinning level 1 propeller with 3 extendable small propeller
MatrixTransform * WeddingCake::createLevel(int level_index)
{
    int propeller_index = level_index * 3;
    int center_post_arm_index = level_index * 6;
    
    MatrixTransform * propeller_1_1 = createPropeller(0 + propeller_index);
    MatrixTransform * propeller_1_2 = createPropeller(1 + propeller_index);
    MatrixTransform * propeller_1_3 = createPropeller(2 + propeller_index);
    
    auto arm1 = center_post_arms[0 + center_post_arm_index];
    auto arm2 = center_post_arms[1 + center_post_arm_index];
    auto arm3 = center_post_arms[2 + center_post_arm_index];
    auto arm4 = center_post_arms[3 + center_post_arm_index];
    auto arm5 = center_post_arms[4 + center_post_arm_index];
    auto arm6 = center_post_arms[5 + center_post_arm_index];
    
    
    glm::mat4 spin_mat = glm::rotate(glm::mat4(1.0f), center_post_spin_angle / 180.0f * glm::pi<float>(), glm::vec3(0, 1, 0));
    glm::mat4 r = glm::rotate(glm::mat4(1.0), 90.0f / 180.0f * glm::pi<float>(), glm::vec3(0, 0, 1));
    glm::mat4 s1 = glm::scale(glm::mat4(1.0), glm::vec3(center_post_propeller_arm_length, center_post_arm_radius, center_post_arm_radius));
    glm::mat4 t1 = glm::translate(glm::mat4(1.0), glm::vec3(center_post_propeller_arm_length/2.0f,0,0));
    glm::mat4 t2 = glm::translate(glm::mat4(1.0), glm::vec3(pingpong_value_center/normalize_factor_center,0,0));
    // Sicnce the arms that holds the pods and the pods are extendable, use pingpong value to make them strech in and out
    glm::mat4 t3 = glm::translate(glm::mat4(1.0), glm::vec3(center_post_propeller_arm_length/2.0f + pingpong_value_center/normalize_factor_center + 0.3f,pods_propeller_elevation,0));
    
    
    // R1 R2 R3 creates a stable non spinning propeller
    MatrixTransform * R1 = new MatrixTransform(glm::rotate(glm::mat4(1.0), 0.0f / 180.0f * glm::pi<float>(), glm::vec3(0, 0, 1)));
    MatrixTransform * R2 = new MatrixTransform(glm::rotate(glm::mat4(1.0), 120.0f / 180.0f * glm::pi<float>(), glm::vec3(0, 1, 0)));
    MatrixTransform * R3 = new MatrixTransform(glm::rotate(glm::mat4(1.0), 240.0f / 180.0f * glm::pi<float>(), glm::vec3(0, 1, 0)));
    
    // Now creates the spinning matrix that spins the level
    MatrixTransform * S = new MatrixTransform(spin_mat);
    
    // Creates a transform holder that holds the spinning level
    MatrixTransform * level = new MatrixTransform(glm::mat4(1.0));
    
    
    arm1->M = t1 * s1 * r;
    arm2->M = t1 * s1 * r;
    arm3->M = t1 * s1 * r;
    
    arm4->M = t2 * s1 * r;
    arm5->M = t2 * s1 * r;
    arm6->M = t2 * s1 * r;
    
    propeller_1_1->M = t3;
    propeller_1_2->M = t3;
    propeller_1_3->M = t3;
    
    level->addChild(S);
    S->addChild(R1);
    S->addChild(R2);
    S->addChild(R3);
    R1->addChild(arm1);
    R2->addChild(arm2);
    R3->addChild(arm3);
    R1->addChild(arm4);
    R2->addChild(arm5);
    R3->addChild(arm6);
    R1->addChild(propeller_1_1);
    R2->addChild(propeller_1_2);
    R3->addChild(propeller_1_3);
    
    return level;
}

// Assembles the wedding cake with a center post and a base
MatrixTransform * WeddingCake::createWeddingCake()
{
    MatrixTransform * cake = new MatrixTransform(glm::mat4(1.0f));
    
    MatrixTransform * centerPost_Translate = new MatrixTransform(glm::translate(glm::mat4(1.0), glm::vec3(0, pingpong_value_center/normalize_factor_center + level_offset * 0, 0)));
    centerPostPingPong->M = glm::scale(glm::mat4(1), glm::vec3(1, center_post_height, 1));

    MatrixTransform * level1_Translate = new MatrixTransform(glm::translate(glm::mat4(1.0), glm::vec3(0, pingpong_value_center/normalize_factor_center + level_offset * -2, 0)));
    MatrixTransform * level1 = createLevel(0);
    
    MatrixTransform * level2_Translate = new MatrixTransform(glm::translate(glm::mat4(1.0), glm::vec3(0, pingpong_value_center/normalize_factor_center + level_offset * -1, 0)));
    MatrixTransform * level2 = createLevel(1);
    
    MatrixTransform * level3_Translate = new MatrixTransform(glm::translate(glm::mat4(1.0), glm::vec3(0, pingpong_value_center/normalize_factor_center + level_offset * 0, 0)));
    MatrixTransform * level3 = createLevel(2);
    
    cake->addChild(centerPost);
    cake->addChild(base);
    cake->addChild(centerPost_Translate);
    cake->addChild(level1_Translate);
    cake->addChild(level2_Translate);
    cake->addChild(level3_Translate);
    centerPost_Translate->addChild(centerPostPingPong);
    level1_Translate->addChild(level1);
    level2_Translate->addChild(level2);
    level3_Translate->addChild(level3);
    
    return cake;
}


void WeddingCake::update()
{
    preUpdate();
    this->weddingCake = createWeddingCake();
    this->weddingCake->update();
    this->weddingCake->draw();
}

void WeddingCake::draw()
{
//    this->weddingCake->draw();
}



