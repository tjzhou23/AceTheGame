#pragma once
#include "cheat_cmd_handler.hpp"
#include "engine_module.hpp"
#include "input.hpp"

/*
 * return value for every cheater mode operation
 *
 * will be used to tell the outer loop like:
 * 1. when to exit cheater mode
 * 2. when and what to change scan type
 * */

struct cheat_on_line_ret {
  E_loop_statement loop_statement = E_loop_statement::continue_;
};
/*
 * core logic of the cheater mode
 * which also handle all the modules
 * needed by it such as [engine_module] of all different type
 *
 * handle scan type changing directly inside cheater mode
 * */
class cheat_session {
private:
  int pid = -1;
  // engine modules for all type
  engine_module<int> *engine_module_ptr_int = NULL;
  engine_module<long> *engine_module_ptr_long = NULL;
  engine_module<short> *engine_module_ptr_short = NULL;
  engine_module<byte> *engine_module_ptr_byte = NULL;
  engine_module<float> *engine_module_ptr_float = NULL;

  // and ect
  // cheat config
  struct cheat_mode_config cheat_config;
  E_num_type current_scan_type;
  //
  struct cheat_on_line_ret current_cheat_on_line_ret;

  // functions

  template <typename T>
  struct cheat_on_line_ret cheater_on_line(engine_module<T> *engine_module_ptr,
                                           cheat_mode_config *cheat_config,
                                           std::string input_str);

  template <typename T>
  struct cheat_on_line_ret
  cheater_mode_on_each_input(int pid, engine_module<T> *engine_module_ptr,
                             struct cheat_mode_config *cheat_config,
                             std::string input_str);

public:
  /*
  * [current_scan_type]: the current scan type to be used
                         which can be changed later on
  */
  cheat_session(int pid, E_num_type current_scan_type);
  ~cheat_session();

  /*
   * the input console of cheater mode
   * whoose input are sent with [input_str]
   *
   * and then the output can be captured
   * by optionally by using [frontend_pop_output]
   * from [to_frontend.hpp]
   *
   * */
  E_loop_statement on_each_input(std::string input_str);
};
