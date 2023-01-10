#include "main_cmd_handler.hpp"
#include "ACE_global.hpp"
#include "aslr_edit.hpp"
#include "cheat.hpp"
#include "file_utils.hpp"
#include "proc_stat.hpp"
#include "to_frontend.hpp"
#include <algorithm>
#include <unistd.h> // for getpid

void process_map_cmd_handler(int pid, bool ps_map_list_all) {

  if (!proc_is_running(pid)) {
    frontend_mark_task_fail("No processes is running with pid %d\n", pid);
    return;
  }
  char path_to_maps[200];
  snprintf(path_to_maps, 199, "/proc/%d/maps", pid);
  //
  std::vector<std::string> proc_maps_file = read_file(path_to_maps);

  size_t special_mapping_count = 0;
  //

  for (size_t i = 0; i < proc_maps_file.size(); i++) {

    struct mem_segment m_seg = parse_proc_map_str(proc_maps_file[i]);
    // by default only show special region
    if (!m_seg.is_special_region && !ps_map_list_all)
      continue;
    printf("%s", m_seg.get_displayable_str().c_str());
    // count special region
    if (m_seg.is_special_region)
      special_mapping_count++;
  }

  printf("------------------------------------\n");
  printf("Found total of %zu mappings\n", proc_maps_file.size());
  printf("With %zu special region mapping\n", special_mapping_count);
  printf("------------------------------------\n");
}

void list_processes_cmd_handler(bool ps_ls_reverse) {

  std::vector<struct proc_info> processes_infos = list_processes();
  /*
   * if second's start time is bigger, then put second in front
   * of first
   *
   * https://stackoverflow.com/a/1380496/14073678
   * */
  std::sort(processes_infos.begin(), processes_infos.end(),

            [ps_ls_reverse](struct proc_info &first, struct proc_info &second) {
              if (ps_ls_reverse)
                return first.start_time > second.start_time;
              else
                return second.start_time > first.start_time;
            }

  );
  // display each of processes
  for (size_t i = 0; i < processes_infos.size(); i++) {
    printf("%d %s\n", processes_infos[i].pid,
           processes_infos[i].proc_name.c_str());
  }
}

void clear_cmd_handler() {

  // clear using escape code for unix based system
  // that is posix
  // https://stackoverflow.com/a/17271636/14073678
  printf("\033[H\033[J");
}

void version_cmd_handler() {

  printf("======================================\n");
  printf("ACE Engine %d.%d.%d\n",

         ACE_global::major_version, ACE_global::minor_version,
         ACE_global::patch_level);

  printf("compile time:  %s  %s\n", __DATE__, __TIME__);
  printf("Compiler : %s %s\n", ACE_global::cpp_compiler_name.c_str(),
         ACE_global::cpp_compiler_version.c_str());
  printf("Endianness: %s\n", ACE_global::platform_endianness_str.c_str());
  printf("======================================\n");
  printf("build options: \n\n\n");

  if (ACE_global::is_android_build)
    printf("built for android\n");
  else
    printf("built for linux desktop\n");

  printf("Features included (+) or not (-): \n");

  printf("%c use \"/proc/<pid>/mem\"\n",
         ACE_global::use_proc_pid_mem ? '+' : '-');

  printf("%c use process_vm_readv and process_vm_writev\n",
         ACE_global::use_proc_vm_read_writev ? '+' : '-');

  printf("======================================\n");
}

void quit_cmd_handler() { exit(EXIT_SUCCESS); };

void cheater_cmd_handler(int pid, E_num_type num_type) {

  // check if <pid> is running
  if (!proc_is_running(pid)) {
    frontend_mark_task_fail("No processes is running with pid %d\n", pid);
    return;
  }

  printf("attaching to process %d \n", pid);
  cheater_mode(pid, num_type);
}

void aslr_cmd_handler(bool aslr_set_val) { aslr_set(aslr_set_val); }

void gui_protocol_cmd_handler() { printf("gui_protocol_ok\n"); }
// ======================================================
void display_intro() { printf("%s", ACE_global::intro_display); }

void license_cmd_handler() {
  printf("%s\n", ACE_global::license);
}
void credit_cmd_handler() { printf("%s\n", ACE_global::engine_credits); }
