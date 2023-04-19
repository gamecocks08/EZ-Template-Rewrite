/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <filesystem>
#include <string>

#include "auton.hpp"
#include "display/lv_core/lv_obj.h"
#include "display/lv_core/lv_style.h"
#include "display/lv_draw/lv_draw_img.h"
#include "display/lv_fonts/lv_font_builtin.h"
#include "display/lv_misc/lv_area.h"
#include "display/lv_misc/lv_color.h"
#include "display/lv_misc/lv_task.h"
#include "display/lv_objx/lv_btn.h"
#include "display/lv_objx/lv_cb.h"
#include "display/lv_objx/lv_img.h"
#include "display/lv_objx/lv_label.h"
#include "display/lv_objx/lv_tabview.h"
#include "display/lv_themes/lv_theme_night.h"
#include "main.h"

#include "icon.hpp"
#include "pros/misc.hpp"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include "util.hpp"

namespace ez::as {
AutonSelector auton_selector{};

static const lv_img_header_t IconImgHead = {
  .cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED,
  .always_zero = 0,
  .w = 480,
  .h = 128,
};

static const lv_img_dsc_t IconImgDsc = {
  .header = IconImgHead,
  .data_size = 128*480*LV_COLOR_DEPTH/8,
  .data = IconData,
};

lv_obj_t * createBtn(lv_obj_t * parent, lv_coord_t x, lv_coord_t y, lv_coord_t width, lv_coord_t height,
    int id, const char * title){
    lv_obj_t * btn = lv_btn_create(parent, NULL);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, width, height);
    lv_obj_set_free_num(btn, id);

    lv_obj_t * label = lv_label_create(btn, NULL);
    lv_label_set_text(label, title);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

    return btn;
}

lv_style_t * createBtnStyle(lv_style_t * copy, lv_color_t rel, lv_color_t pr,
    lv_color_t tglRel, lv_color_t tglPr, lv_color_t tglBorder, lv_color_t textColor, lv_obj_t * btn)
{
    lv_style_t * btnStyle = (lv_style_t *)malloc(sizeof(lv_style_t) * 4);

    for(int i = 0; i < 4; i++) lv_style_copy(&btnStyle[i], copy);

    btnStyle[0].body.main_color = rel;
    btnStyle[0].body.grad_color = rel;
    btnStyle[0].text.color = textColor;

    btnStyle[1].body.main_color = pr;
    btnStyle[1].body.grad_color = pr;
    btnStyle[1].text.color = textColor;

    btnStyle[2].body.main_color = tglRel;
    btnStyle[2].body.grad_color = tglRel;
    btnStyle[2].body.border.width = 2;
    btnStyle[2].body.border.color = tglBorder;
    btnStyle[2].text.color = textColor;

    btnStyle[3].body.main_color = tglPr;
    btnStyle[3].body.grad_color = tglPr;
    btnStyle[3].body.border.width = 2;
    btnStyle[3].body.border.color = tglBorder;
    btnStyle[3].text.color = textColor;

    lv_btn_set_style(btn, LV_BTN_STYLE_REL, &btnStyle[0]);
    lv_btn_set_style(btn, LV_BTN_STYLE_PR, &btnStyle[1]);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, &btnStyle[2]);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, &btnStyle[3]);

    return btnStyle;
}

void setBtnStyle(lv_style_t * btnStyle, lv_obj_t * btn)
{
    lv_btn_set_style(btn, LV_BTN_STYLE_REL, &btnStyle[0]);
    lv_btn_set_style(btn, LV_BTN_STYLE_PR, &btnStyle[1]);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, &btnStyle[2]);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, &btnStyle[3]);
}

lv_obj_t * tabview = lv_tabview_create(lv_scr_act(), NULL);

lv_obj_t * AutonTab = lv_tabview_add_tab(tabview, "Auton");
lv_obj_t * HomeTab = lv_tabview_add_tab(tabview, "Home");
lv_obj_t * SettingsTab = lv_tabview_add_tab(tabview, "Settings");

lv_obj_t * AutonSelect = lv_tabview_create(AutonTab, NULL);

lv_obj_t * ShortTab = lv_tabview_add_tab(AutonSelect, "Short");
lv_obj_t * LongTab = lv_tabview_add_tab(AutonSelect, "Long");
lv_obj_t * SkillsTab = lv_tabview_add_tab(AutonSelect, "Skills");

//lv_obj_t * IconAuton = lv_img_create(AutonTab, NULL);
//lv_obj_t * IconHome = lv_img_create(HomeTab, NULL);
//lv_obj_t * IconSettings = lv_img_create(SettingsTab, NULL);

//lv_obj_t * Auton = lv_label_create(AutonTab, NULL);
lv_obj_t * Bat_Level = lv_label_create(HomeTab, NULL);;

//lv_obj_t * Next_But = createBtn(AutonTab, 20, 100, 100, 60, 1, "->");
//lv_obj_t * Prev_But = createBtn(AutonTab, 20, 180, 100, 60, 2, "<-");

/*
lv_style_t * Next_Style = createBtnStyle(
  &lv_style_plain, 
  LV_COLOR_MAKE(6, 84, 95), 
  LV_COLOR_MAKE(0, 0 ,0), 
  LV_COLOR_MAKE(255, 0 ,0), 
  LV_COLOR_MAKE(255, 0 ,0), 
  LV_COLOR_MAKE(255, 0 ,0), 
  LV_COLOR_MAKE(255, 255, 255), 
  Next_But);

lv_style_t * Prev_Style = createBtnStyle(
  &lv_style_plain, 
  LV_COLOR_MAKE(6, 84, 95), 
  LV_COLOR_MAKE(0, 0 ,0), 
  LV_COLOR_MAKE(255, 0 ,0), 
  LV_COLOR_MAKE(255, 0 ,0), 
  LV_COLOR_MAKE(255, 0 ,0), 
  LV_COLOR_MAKE(255, 255, 255), 
  Prev_But);
*/
lv_theme_t * TabButtonTheme = lv_theme_night_init(0, NULL);
lv_theme_t * AutonSelectTheme = lv_theme_night_init(0, NULL);

static lv_res_t event_handler(lv_obj_t * btn){
  uint8_t id = lv_obj_get_free_num(btn);

  if(id==1){
    ez::as::page_up();
  }else if(id==2){
    ez::as::page_down();
  }

  return LV_RES_OK;
}

void Update_Bat_Level (lv_obj_t * Field) {
  while(true){
    lv_label_set_text(Field, ("Battery Level: " + std::to_string(pros::battery::get_capacity())).c_str());
    pros::delay(500);
  }
}



void update_auto_sd() {
  // If no SD card, return
  if (!ez::util::IS_SD_CARD) return;

  FILE* usd_file_write = fopen("/usd/auto.txt", "w");
  std::string cp_str = std::to_string(auton_selector.current_auton_page);
  char const* cp_c = cp_str.c_str();
  fputs(cp_c, usd_file_write);
  fclose(usd_file_write);
}

void init_auton_selector() {
  // If no SD card, return
  if (!ez::util::IS_SD_CARD) return;

  FILE* as_usd_file_read;
  // If file exists...
  if ((as_usd_file_read = fopen("/usd/auto.txt", "r"))) {
    char l_buf[5];
    fread(l_buf, 1, 5, as_usd_file_read);
    ez::as::auton_selector.current_auton_page = std::stof(l_buf);
    fclose(as_usd_file_read);
  }
  // If file doesn't exist, create file
  else {
    update_auto_sd();  // Writing to a file that doesn't exist creates the file
    printf("Created auto.txt\n");
  }

  if (ez::as::auton_selector.current_auton_page > ez::as::auton_selector.auton_count - 1 || ez::as::auton_selector.current_auton_page < 0) {
    ez::as::auton_selector.current_auton_page = 0;
    ez::as::update_auto_sd();
  }
}

void page_up() {
  if (auton_selector.current_auton_page == auton_selector.auton_count - 1)
    auton_selector.current_auton_page = 0;
  else
    auton_selector.current_auton_page++;
  update_auto_sd();
  //auton_selector.print_selected_auton(Auton);
}

void page_down() {
  if (auton_selector.current_auton_page == 0)
    auton_selector.current_auton_page = auton_selector.auton_count - 1;
  else
    auton_selector.current_auton_page--;
  update_auto_sd();
  //auton_selector.print_selected_auton(Auton);
}

void Boot_Up () {
  pros::Task battery_level_task([&] { Update_Bat_Level(Bat_Level); });
  //lv_img_set_src(IconAuton, &IconImgDsc);
  //lv_img_set_src(IconHome, &IconImgDsc);
  //lv_img_set_src(IconSettings, &IconImgDsc);
  //lv_obj_align(Auton, NULL, LV_ALIGN_CENTER, -70, 20);
  lv_obj_align(Bat_Level, NULL, LV_ALIGN_CENTER, -70, 20);
  //lv_btn_set_action(Next_But, LV_BTN_ACTION_CLICK, event_handler);
  //lv_btn_set_action(Prev_But, LV_BTN_ACTION_CLICK, event_handler);

  lv_obj_set_size(AutonSelect, 480, 60);

  AutonSelectTheme->tabview.btn.tgl_rel->body.main_color = LV_COLOR_BLACK;
  AutonSelectTheme->tabview.btn.tgl_rel->body.grad_color = LV_COLOR_BLACK;
  AutonSelectTheme->tabview.btn.tgl_rel->text.color = LV_COLOR_WHITE;

  AutonSelectTheme->tabview.btn.bg->body.padding.hor = 2;
  AutonSelectTheme->tabview.btn.bg->body.padding.ver = 2;
  AutonSelectTheme->tabview.btn.bg->body.padding.inner = 2;

  AutonSelectTheme->tabview.indic->body.main_color = LV_COLOR_MAKE(6, 84, 95);
  AutonSelectTheme->tabview.indic->body.grad_color = LV_COLOR_MAKE(6, 84, 95);

  TabButtonTheme->tabview.btn.tgl_rel->body.main_color = LV_COLOR_BLACK;
  TabButtonTheme->tabview.btn.tgl_rel->body.grad_color = LV_COLOR_BLACK;
  TabButtonTheme->tabview.btn.tgl_rel->text.color = LV_COLOR_WHITE;

  TabButtonTheme->tabview.btn.bg->body.padding.hor = 2;
  TabButtonTheme->tabview.btn.bg->body.padding.ver = 2;
  TabButtonTheme->tabview.btn.bg->body.padding.inner = 2;

  TabButtonTheme->tabview.indic->body.main_color = LV_COLOR_MAKE(6, 84, 95);
  TabButtonTheme->tabview.indic->body.grad_color = LV_COLOR_MAKE(6, 84, 95);
  
  lv_tabview_set_style(AutonSelect, LV_TABVIEW_STYLE_BTN_TGL_REL, AutonSelectTheme->tabview.btn.tgl_rel);
  lv_tabview_set_style(AutonSelect, LV_TABVIEW_STYLE_BTN_BG, AutonSelectTheme->tabview.btn.bg);
  lv_tabview_set_style(AutonSelect, LV_TABVIEW_STYLE_INDIC, AutonSelectTheme->tabview.indic);
  lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_INDIC, TabButtonTheme->tabview.indic);
  lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_TGL_REL, TabButtonTheme->tabview.btn.tgl_rel);
  lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_BG, TabButtonTheme->tabview.btn.bg);

}

void initialize() {
  // Initialize auto selector and LVGL
  ez::as::init_auton_selector();
  // Callbacks for auto selector
  //ez::as::auton_selector.print_selected_auton(Auton);
}

void shutdown() {
  //pros::lcd::shutdown();
}

bool turn_off = false;

// Using a button to control the lcd
pros::ADIDigitalIn* left_limit_switch = nullptr;
pros::ADIDigitalIn* right_limit_switch = nullptr;
pros::Task limit_switch_task(limitSwitchTask);
void limit_switch_lcd_initialize(pros::ADIDigitalIn* right_limit, pros::ADIDigitalIn* left_limit) {
  if (!left_limit && !right_limit) {
    delete left_limit_switch;
    delete right_limit_switch;
    if (pros::millis() <= 100)
      turn_off = true;
    return;
  }
  turn_off = false;
  right_limit_switch = right_limit;
  left_limit_switch = left_limit;
  limit_switch_task.resume();
}

void limitSwitchTask() {
  while (true) {
    if (right_limit_switch && right_limit_switch->get_new_press())
      page_up();
    else if (left_limit_switch && left_limit_switch->get_new_press())
      page_down();

    if (pros::millis() >= 500 && turn_off)
      limit_switch_task.suspend();

    pros::delay(50);
  }
}
}  // namespace ez::as
