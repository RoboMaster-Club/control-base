//
// Created by RM UI Designer
//

#include "ui_indicator_0_10.h"

#define FRAME_ID 1
#define GROUP_ID 0
#define START_ID 10
#define OBJ_NUM 5
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_indicator_0_10;
ui_interface_round_t *ui_indicator_0_Aim_Circle = (ui_interface_round_t *)&(ui_indicator_0_10.data[0]);
ui_interface_line_t *ui_indicator_0_Aim_V_Line = (ui_interface_line_t *)&(ui_indicator_0_10.data[1]);
ui_interface_line_t *ui_indicator_0_Aim_H_Line = (ui_interface_line_t *)&(ui_indicator_0_10.data[2]);
ui_interface_line_t *ui_indicator_0_Pathway_Left = (ui_interface_line_t *)&(ui_indicator_0_10.data[3]);
ui_interface_line_t *ui_indicator_0_Pathway_Right = (ui_interface_line_t *)&(ui_indicator_0_10.data[4]);

void _ui_init_indicator_0_10() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_indicator_0_10.data[i].figure_name[0] = FRAME_ID;
        ui_indicator_0_10.data[i].figure_name[1] = GROUP_ID;
        ui_indicator_0_10.data[i].figure_name[2] = i + START_ID;
        ui_indicator_0_10.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_indicator_0_10.data[i].operate_tpyel = 0;
    }

    ui_indicator_0_Aim_Circle->figure_tpye = 2;
    ui_indicator_0_Aim_Circle->layer = 0;
    ui_indicator_0_Aim_Circle->r = 50;
    ui_indicator_0_Aim_Circle->start_x = 900;
    ui_indicator_0_Aim_Circle->start_y = 500;
    ui_indicator_0_Aim_Circle->color = 3;
    ui_indicator_0_Aim_Circle->width = 6;

    ui_indicator_0_Aim_V_Line->figure_tpye = 0;
    ui_indicator_0_Aim_V_Line->layer = 0;
    ui_indicator_0_Aim_V_Line->start_x = 900;
    ui_indicator_0_Aim_V_Line->start_y = 465;
    ui_indicator_0_Aim_V_Line->end_x = 900;
    ui_indicator_0_Aim_V_Line->end_y = 538;
    ui_indicator_0_Aim_V_Line->color = 3;
    ui_indicator_0_Aim_V_Line->width = 4;

    ui_indicator_0_Aim_H_Line->figure_tpye = 0;
    ui_indicator_0_Aim_H_Line->layer = 0;
    ui_indicator_0_Aim_H_Line->start_x = 865;
    ui_indicator_0_Aim_H_Line->start_y = 500;
    ui_indicator_0_Aim_H_Line->end_x = 938;
    ui_indicator_0_Aim_H_Line->end_y = 500;
    ui_indicator_0_Aim_H_Line->color = 3;
    ui_indicator_0_Aim_H_Line->width = 4;

    ui_indicator_0_Pathway_Left->figure_tpye = 0;
    ui_indicator_0_Pathway_Left->layer = 0;
    ui_indicator_0_Pathway_Left->start_x = 550;
    ui_indicator_0_Pathway_Left->start_y = 0;
    ui_indicator_0_Pathway_Left->end_x = 649;
    ui_indicator_0_Pathway_Left->end_y = 350;
    ui_indicator_0_Pathway_Left->color = 1;
    ui_indicator_0_Pathway_Left->width = 4;

    ui_indicator_0_Pathway_Right->figure_tpye = 0;
    ui_indicator_0_Pathway_Right->layer = 0;
    ui_indicator_0_Pathway_Right->start_x = 1349;
    ui_indicator_0_Pathway_Right->start_y = 0;
    ui_indicator_0_Pathway_Right->end_x = 1250;
    ui_indicator_0_Pathway_Right->end_y = 350;
    ui_indicator_0_Pathway_Right->color = 1;
    ui_indicator_0_Pathway_Right->width = 4;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_indicator_0_10);
    SEND_MESSAGE((uint8_t *) &ui_indicator_0_10, sizeof(ui_indicator_0_10));
}

void _ui_update_indicator_0_10() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_indicator_0_10.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_indicator_0_10);
    SEND_MESSAGE((uint8_t *) &ui_indicator_0_10, sizeof(ui_indicator_0_10));
}

void _ui_remove_indicator_0_10() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_indicator_0_10.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_indicator_0_10);
    SEND_MESSAGE((uint8_t *) &ui_indicator_0_10, sizeof(ui_indicator_0_10));
}
