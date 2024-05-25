//
// Created by RM UI Designer
//

#include "ui_indicator_0_10.h"

#define FRAME_ID 1
#define GROUP_ID 0
#define START_ID 10
#define OBJ_NUM 4
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_indicator_0_10;
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

    ui_indicator_0_Aim_V_Line->figure_tpye = 0;
    ui_indicator_0_Aim_V_Line->layer = 0;
    ui_indicator_0_Aim_V_Line->start_x = 925;
    ui_indicator_0_Aim_V_Line->start_y = 455;
    ui_indicator_0_Aim_V_Line->end_x = 925;
    ui_indicator_0_Aim_V_Line->end_y = 485;
    ui_indicator_0_Aim_V_Line->color = 3;
    ui_indicator_0_Aim_V_Line->width = 2;

    ui_indicator_0_Aim_H_Line->figure_tpye = 0;
    ui_indicator_0_Aim_H_Line->layer = 0;
    ui_indicator_0_Aim_H_Line->start_x = 910;
    ui_indicator_0_Aim_H_Line->start_y = 470;
    ui_indicator_0_Aim_H_Line->end_x = 940;
    ui_indicator_0_Aim_H_Line->end_y = 470;
    ui_indicator_0_Aim_H_Line->color = 3;
    ui_indicator_0_Aim_H_Line->width = 2;

    ui_indicator_0_Pathway_Left->figure_tpye = 0;
    ui_indicator_0_Pathway_Left->layer = 0;
    ui_indicator_0_Pathway_Left->start_x = 510;
    ui_indicator_0_Pathway_Left->start_y = 0;
    ui_indicator_0_Pathway_Left->end_x = 720;
    ui_indicator_0_Pathway_Left->end_y = 350;
    ui_indicator_0_Pathway_Left->color = 1;
    ui_indicator_0_Pathway_Left->width = 4;

    ui_indicator_0_Pathway_Right->figure_tpye = 0;
    ui_indicator_0_Pathway_Right->layer = 0;
    ui_indicator_0_Pathway_Right->start_x = 1300;
    ui_indicator_0_Pathway_Right->start_y = 0;
    ui_indicator_0_Pathway_Right->end_x = 1090;
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
