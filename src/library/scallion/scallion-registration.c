/**
 * Scallion - plug-in for The Shadow Simulator
 *
 * Copyright (c) 2010-2011 Rob Jansen <jansen@cs.umn.edu>
 *
 * This file is part of Scallion.
 *
 * Scallion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Scallion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Scallion.  If not, see <http://www.gnu.org/licenses/>.
 *
 * NOTE: THIS FILE AUTOMATICALLY GENERATED BY SETUP SCRIPT!
 */

const char tor_git_revision[] = "";

#include "scallion.h"

#include "tor_includes.h"
#include "tor_externs.h"

void scallion_register_globals(PluginFunctionTable* scallionFuncs, Scallion* scallionData) {
	scallionData->shadowlibFuncs->registerPlugin(scallionFuncs, 431,
		(gsize) sizeof(Scallion), (gpointer) scallionData,
		(gsize) 0x4, (gpointer) &gzip_is_supported,
		(gsize) 0x68, (gpointer) &orconn_circid_map_PRIMES,
		(gsize) 0x8, (gpointer) &stats_n_padding_cells_processed,
		(gsize) 0x8, (gpointer) &total_n_extend,
		(gsize) 0x8, (gpointer) &n_bytes_read_in_interval,
		(gsize) 0x18, (gpointer) &last_warned1,
		(gsize) 0x8, (gpointer) &last_warned2,
		(gsize) 0x8, (gpointer) &interval_wakeup_time,
		(gsize) 0x8, (gpointer) &stats_n_relay_cells_processed,
		(gsize) 0x4, (gpointer) &digestmap_impl_N_PRIMES,
		(gsize) 0x8, (gpointer) &stats_n_data_cells_packaged,
		(gsize) 0x4, (gpointer) &n_bogus_times,
		(gsize) 0x8, (gpointer) &ewma_scale_factor,
		(gsize) 0x8, (gpointer) &warned_nonexistent_family,
		(gsize) 0x8, (gpointer) &dh_param_p_tls,
		(gsize) 0x4, (gpointer) &num_seen,
		(gsize) 0x8, (gpointer) &managed_proxy_list,
		(gsize) 0x18, (gpointer) &loop_warn_limit,
		(gsize) 0x8, (gpointer) &virtaddress_reversemap,
		(gsize) 0x4, (gpointer) &log_domains_are_logged,
		(gsize) 0x68, (gpointer) &nodelist_map_PRIMES,
		(gsize) 0x8, (gpointer) &last_rotation_time,
		(gsize) 0x4, (gpointer) &tor_tls_object_ex_data_index,
		(gsize) 0x8, (gpointer) &resolv_conf_fname,
		(gsize) 0x8, (gpointer) &time_to_write_stats_files,
		(gsize) 0x4, (gpointer) &disabled_debugger_attach,
		(gsize) 0x8, (gpointer) &last_interface_ipv6,
		(gsize) 0x8, (gpointer) &time_to_retry_dns_init,
		(gsize) 0x8, (gpointer) &unnamed_server_map,
		(gsize) 0x8, (gpointer) &global_options,
		(gsize) 0x18, (gpointer) &socks_ratelim,
		(gsize) 0x4, (gpointer) &ever_answered_yes,
		(gsize) 0x4, (gpointer) &n_seconds_to_hit_soft_limit,
		(gsize) 0x8, (gpointer) &child_handle,
		(gsize) 0x4, (gpointer) &dirport_reachability_count,
		(gsize) 0x8, (gpointer) &networkstatus_v2_list,
		(gsize) 0x8, (gpointer) &last_rotated_x509_certificate,
		(gsize) 0x8, (gpointer) &stats_n_certs_cells_processed,
		(gsize) 0x8, (gpointer) &owning_controller_process_spec,
		(gsize) 0x4, (gpointer) &guard_bandwidth_excluding_exits,
		(gsize) 0x8, (gpointer) &cached_consensuses,
		(gsize) 0x8, (gpointer) &bridge_list,
		(gsize) 0x4, (gpointer) &authentication_cookie_is_set,
		(gsize) 0x8, (gpointer) &configured_ports,
		(gsize) 0x8, (gpointer) &stable_mtbf,
		(gsize) 0x1, (gpointer) &ctr,
		(gsize) 0x4, (gpointer) &networkstatus_v2_list_has_changed,
		(gsize) 0x8, (gpointer) &time_to_clean_caches,
		(gsize) 0x4, (gpointer) &advertising,
		(gsize) 0x18, (gpointer) &ns_v3_responses,
		(gsize) 0x18, (gpointer) &policy_root,
		(gsize) 0x8, (gpointer) &time_to_reset_descriptor_failures,
		(gsize) 0x8, (gpointer) &last_written_bytes_noted,
		(gsize) 0x4, (gpointer) &libevent_initialized,
		(gsize) 0x8, (gpointer) &the_directory_is_dirty,
		(gsize) 0x4, (gpointer) &cfg_unit,
		(gsize) 0x8, (gpointer) &current_md_consensus,
		(gsize) 0x8, (gpointer) &appname,
		(gsize) 0x4, (gpointer) &have_performed_bandwidth_test,
		(gsize) 0x4, (gpointer) &check_if_restarts_needed,
		(gsize) 0x8, (gpointer) &n_bytes_written_in_interval,
		(gsize) 0x4, (gpointer) &strmap_impl_N_PRIMES,
		(gsize) 0x8, (gpointer) &time_to_try_getting_descriptors,
		(gsize) 0x8, (gpointer) &start_of_served_descs_stats_interval,
		(gsize) 0x8, (gpointer) &the_directory,
		(gsize) 0x8, (gpointer) &microdesc_consensus_methods,
		(gsize) 0x8, (gpointer) &ol_tail,
		(gsize) 0x4, (gpointer) &global_write_bucket,
		(gsize) 0x10, (gpointer) &cached_time_hires,
		(gsize) 0x4, (gpointer) &should_add_entry_nodes,
		(gsize) 0x4, (gpointer) &max_fd,
		(gsize) 0x8, (gpointer) &last_desc_dumped,
		(gsize) 0x18, (gpointer) &orconn_circid_circuit_map,
		(gsize) 0x4, (gpointer) &disable_log_messages,
		(gsize) 0x8, (gpointer) &stats_n_authenticate_cells_processed,
		(gsize) 0x8, (gpointer) &second_timer,
		(gsize) 0x4, (gpointer) &_log_global_min_severity,
		(gsize) 0x18, (gpointer) &bidi_map,
		(gsize) 0x8, (gpointer) &last_culled_cpuworkers,
		(gsize) 0x20, (gpointer) &pending_consensuses,
		(gsize) 0x18, (gpointer) &delay_limit,
		(gsize) 0x8, (gpointer) &bidi_next_interval,
		(gsize) 0x4, (gpointer) &ol_length,
		(gsize) 0x8, (gpointer) &cached_v2_networkstatus,
		(gsize) 0x8, (gpointer) &the_v2_networkstatus,
		(gsize) 0x8, (gpointer) &stats_n_vpadding_cells_processed,
		(gsize) 0x8, (gpointer) &server_tls_context,
		(gsize) 0x8, (gpointer) &runningrouters_is_dirty,
		(gsize) 0x50, (gpointer) &first_good_0_2_2_guard_version,
		(gsize) 0x4, (gpointer) &memory_lock_attempted,
		(gsize) 0x4, (gpointer) &arrays_initialized,
		(gsize) 0x8, (gpointer) &torrc_defaults_fname,
		(gsize) 0x8, (gpointer) &bridge_stats_extrainfo,
		(gsize) 0x14, (gpointer) &geoip_digest,
		(gsize) 0x4, (gpointer) &newnym_epoch,
		(gsize) 0x8, (gpointer) &time_to_launch_reachability_tests,
		(gsize) 0x8, (gpointer) &socket_accounting_mutex,
		(gsize) 0x8, (gpointer) &last_time_determined_shares,
		(gsize) 0x8, (gpointer) &authdir_reject_policy,
		(gsize) 0x8, (gpointer) &stats_n_netinfo_cells_processed,
		(gsize) 0x8, (gpointer) &trusted_dir_certs,
		(gsize) 0x8, (gpointer) &auth_hid_servs,
		(gsize) 0x8, (gpointer) &last_dummy_download,
		(gsize) 0x8, (gpointer) &next_id,
		(gsize) 0x8, (gpointer) &read_array,
		(gsize) 0x8, (gpointer) &history_map,
		(gsize) 0x4, (gpointer) &quiet_level,
		(gsize) 0x50, (gpointer) &first_good_later_guard_version,
		(gsize) 0x4, (gpointer) &n_circuit_failures,
		(gsize) 0x8, (gpointer) &suppress_msg,
		(gsize) 0x80, (gpointer) &signal_events,
		(gsize) 0x4, (gpointer) &mostly_written,
		(gsize) 0x8, (gpointer) &time_to_shrink_memory,
		(gsize) 0x4, (gpointer) &unit_tests,
		(gsize) 0x70, (gpointer) &the_runningrouters,
		(gsize) 0x8, (gpointer) &stats_n_destroy_cells_processed,
		(gsize) 0x4, (gpointer) &nodelist_map_N_PRIMES,
		(gsize) 0x4, (gpointer) &enough_mtbf_info,
		(gsize) 0x4, (gpointer) &nameservers_configured,
		(gsize) 0x8, (gpointer) &exit_streams,
		(gsize) 0x4, (gpointer) &num_cpuworkers_busy,
		(gsize) 0x8, (gpointer) &pending_consensus_signatures,
		(gsize) 0x8, (gpointer) &desc_dirty_reason,
		(gsize) 0x8, (gpointer) &hibernate_end_time,
		(gsize) 0x8, (gpointer) &time_to_downrate_stability,
		(gsize) 0x8, (gpointer) &reachable_or_addr_policy,
		(gsize) 0x4, (gpointer) &next_virtual_addr,
		(gsize) 0x18, (gpointer) &priv_warn_limit,
		(gsize) 0x4, (gpointer) &dns_is_completely_invalid,
		(gsize) 0x8, (gpointer) &stats_n_relay_cells_relayed,
		(gsize) 0x8, (gpointer) &rend_service_list,
		(gsize) 0x8, (gpointer) &total_bandwidth,
		(gsize) 0x8, (gpointer) &CLIENT_CIPHER_DUMMIES,
		(gsize) 0x8, (gpointer) &_openssl_mutexes,
		(gsize) 0x4, (gpointer) &have_warned_about_new_version,
		(gsize) 0x40, (gpointer) &consensus_waiting_for_certs,
		(gsize) 0x4, (gpointer) &share_seconds,
		(gsize) 0x8, (gpointer) &client_identitykey,
		(gsize) 0x4, (gpointer) &log_mutex_initialized,
		(gsize) 0x8, (gpointer) &v3_share_times_seconds,
		(gsize) 0x8, (gpointer) &the_microdesc_cache,
		(gsize) 0x8, (gpointer) &onionkey,
		(gsize) 0x4, (gpointer) &entry_guards_dirty,
		(gsize) 0x8, (gpointer) &desc_extrainfo,
		(gsize) 0x4, (gpointer) &n_libevent_errors,
		(gsize) 0x4, (gpointer) &did_circs_fail_last_period,
		(gsize) 0x8, (gpointer) &total_descriptor_downloads,
		(gsize) 0x8, (gpointer) &n_freelist_miss,
		(gsize) 0x8, (gpointer) &time_to_check_for_correct_dns,
		(gsize) 0x8, (gpointer) &cached_resolve_pqueue,
		(gsize) 0x8, (gpointer) &last_read_bytes_noted,
		(gsize) 0x8, (gpointer) &CLIENT_CIPHER_STACK,
		(gsize) 0x8, (gpointer) &cell_pool,
		(gsize) 0x8, (gpointer) &total_exit_bandwidth,
		(gsize) 0x8, (gpointer) &last_time_noted,
		(gsize) 0x8, (gpointer) &owning_controller_process_monitor,
		(gsize) 0x8, (gpointer) &stats_n_data_bytes_received,
		(gsize) 0x8, (gpointer) &entry_guards,
		(gsize) 0x8, (gpointer) &interval_start_time,
		(gsize) 0x4, (gpointer) &total_cells_allocated,
		(gsize) 0x14, (gpointer) &last_guessed_ip,
		(gsize) 0x8, (gpointer) &last_hid_serv_requests_,
		(gsize) 0x8, (gpointer) &launch_event,
		(gsize) 0x4, (gpointer) &parsed_versions_initialized,
		(gsize) 0x68, (gpointer) &microdesc_map_PRIMES,
		(gsize) 0x8, (gpointer) &time_to_check_descriptor,
		(gsize) 0x8, (gpointer) &the_short_tor_version,
		(gsize) 0x8, (gpointer) &time_to_write_bridge_stats,
		(gsize) 0x8, (gpointer) &last_descriptor_download_attempted,
		(gsize) 0x8, (gpointer) &start_of_dirreq_stats_interval,
		(gsize) 0x4, (gpointer) &should_init_bridge_stats,
		(gsize) 0x8, (gpointer) &reachable_dir_addr_policy,
		(gsize) 0x8, (gpointer) &stats_n_bytes_read,
		(gsize) 0x4, (gpointer) &bootstrap_percent,
		(gsize) 0x4, (gpointer) &can_complete_circuit,
		(gsize) 0x8, (gpointer) &time_to_check_for_expired_networkstatus,
		(gsize) 0x4, (gpointer) &nameserver_config_failed,
		(gsize) 0x4, (gpointer) &stats_prev_global_write_bucket,
		(gsize) 0x4, (gpointer) &cache_map_N_PRIMES,
		(gsize) 0x8, (gpointer) &pending_consensus_signature_list,
		(gsize) 0x8, (gpointer) &start_of_bridge_stats_interval,
		(gsize) 0x4, (gpointer) &global_relayed_write_bucket,
		(gsize) 0x8, (gpointer) &exit_bytes_read,
		(gsize) 0x4, (gpointer) &virtual_addr_network,
		(gsize) 0x4, (gpointer) &bidimap_N_PRIMES,
		(gsize) 0x4, (gpointer) &n_wildcard_requests,
		(gsize) 0x8, (gpointer) &outgoing_addrs,
		(gsize) 0x4, (gpointer) &uname_result_is_set,
		(gsize) 0x8, (gpointer) &soft_limit_hit_at,
		(gsize) 0x68, (gpointer) &policy_map_PRIMES,
		(gsize) 0x8, (gpointer) &predicted_internal_time,
		(gsize) 0x4, (gpointer) &num_cpuworkers,
		(gsize) 0x4, (gpointer) &microdesc_map_N_PRIMES,
		(gsize) 0x18, (gpointer) &client_history,
		(gsize) 0x4, (gpointer) &all_down,
		(gsize) 0x8, (gpointer) &routerlist,
		(gsize) 0x14, (gpointer) &server_identitykey_digest,
		(gsize) 0x8, (gpointer) &last_changed,
		(gsize) 0x4, (gpointer) &ewma_enabled,
		(gsize) 0x4, (gpointer) &mostly_read,
		(gsize) 0x8, (gpointer) &desc_routerinfo,
		(gsize) 0x8, (gpointer) &time_to_recheck_bandwidth,
		(gsize) 0x8, (gpointer) &stats_n_data_bytes_packaged,
		(gsize) 0x8, (gpointer) &transport_list,
		(gsize) 0x71, (gpointer) &buf8,
		(gsize) 0x10, (gpointer) &poll_interval_tv,
		(gsize) 0x4, (gpointer) &cfg_start_min,
		(gsize) 0x10, (gpointer) &buf1,
		(gsize) 0x40, (gpointer) &buf3,
		(gsize) 0x41, (gpointer) &buf2,
		(gsize) 0x20, (gpointer) &buf5,
		(gsize) 0x4, (gpointer) &policy_map_N_PRIMES,
		(gsize) 0x60, (gpointer) &buf7,
		(gsize) 0x40, (gpointer) &buf6,
		(gsize) 0x71, (gpointer) &buf9,
		(gsize) 0x4, (gpointer) &unconfigured_proxies_n,
		(gsize) 0x8, (gpointer) &pending_vote_list,
		(gsize) 0x4, (gpointer) &rephist_total_num,
		(gsize) 0x8, (gpointer) &v2_share_times_seconds,
		(gsize) 0x4, (gpointer) &disable_broken_connection_counts,
		(gsize) 0x4, (gpointer) &threads_initialized,
		(gsize) 0x8, (gpointer) &global_circuitlist,
		(gsize) 0x8, (gpointer) &onionkey_set_at,
		(gsize) 0x8, (gpointer) &torrc_fname,
		(gsize) 0x118, (gpointer) &freelists,
		(gsize) 0x8, (gpointer) &rephist_total_alloc,
		(gsize) 0x30, (gpointer) &buf,
		(gsize) 0x8, (gpointer) &circuits_pending_or_conns,
		(gsize) 0x4, (gpointer) &write_stats_to_extrainfo,
		(gsize) 0x10, (gpointer) &current_millisecond,
		(gsize) 0x8, (gpointer) &main_thread_id,
		(gsize) 0x8, (gpointer) &it_pool,
		(gsize) 0x8, (gpointer) &time_to_new_circuit,
		(gsize) 0x4, (gpointer) &n_sockets_open,
		(gsize) 0x4, (gpointer) &desc_needs_upload,
		(gsize) 0x8, (gpointer) &stats_n_relay_cells_delivered,
		(gsize) 0x4, (gpointer) &dns_wildcard_notice_given,
		(gsize) 0x68, (gpointer) &clientmap_PRIMES,
		(gsize) 0x8, (gpointer) &logfiles,
		(gsize) 0x8, (gpointer) &dir_read_array,
		(gsize) 0x8, (gpointer) &cached_approx_time,
		(gsize) 0x28, (gpointer) &log_mutex,
		(gsize) 0x8, (gpointer) &time_to_check_listeners,
		(gsize) 0x18, (gpointer) &handoff_warning,
		(gsize) 0x8, (gpointer) &cached_directory,
		(gsize) 0x8, (gpointer) &sl_last_weighted_bw_of_me,
		(gsize) 0x8, (gpointer) &stability_last_downrated,
		(gsize) 0x8, (gpointer) &built_last_stability_doc_at,
		(gsize) 0x8, (gpointer) &desc_gen_reason,
		(gsize) 0x8, (gpointer) &connection_array,
		(gsize) 0x8, (gpointer) &stats_n_auth_challenge_cells_processed,
		(gsize) 0x8, (gpointer) &stats_prev_n_read,
		(gsize) 0x8, (gpointer) &closeable_connection_lst,
		(gsize) 0x18, (gpointer) &warning_limit,
		(gsize) 0x4, (gpointer) &stats_prev_global_read_bucket,
		(gsize) 0x8, (gpointer) &time_of_process_start,
		(gsize) 0x8, (gpointer) &the_v2_networkstatus_is_dirty,
		(gsize) 0x18, (gpointer) &zero_bandwidth_warning_limit,
		(gsize) 0x8, (gpointer) &_escaped_val,
		(gsize) 0x8, (gpointer) &interval_end_time,
		(gsize) 0x8, (gpointer) &circuits_for_buffer_stats,
		(gsize) 0x4, (gpointer) &have_min_dir_info,
		(gsize) 0x8, (gpointer) &the_event_base,
		(gsize) 0x8, (gpointer) &orconn_identity_map,
		(gsize) 0x8, (gpointer) &the_tor_version,
		(gsize) 0x8, (gpointer) &dns_wildcard_list,
		(gsize) 0x8, (gpointer) &daemon_filedes,
		(gsize) 0x4, (gpointer) &dns_wildcarded_test_address_notice_given,
		(gsize) 0x4, (gpointer) &last_resolved_addr,
		(gsize) 0x8, (gpointer) &time_to_save_stability,
		(gsize) 0x8, (gpointer) &last_stability_doc,
		(gsize) 0x8, (gpointer) &predicted_internal_uptime_time,
		(gsize) 0x8, (gpointer) &time_to_add_entropy,
		(gsize) 0x8, (gpointer) &authdir_invalid_policy,
		(gsize) 0x4, (gpointer) &dirreqmap_N_PRIMES,
		(gsize) 0x4, (gpointer) &clientmap_N_PRIMES,
		(gsize) 0x8, (gpointer) &stats_n_created_cells_processed,
		(gsize) 0x8, (gpointer) &refill_timer,
		(gsize) 0x8, (gpointer) &time_to_write_bridge_status_file,
		(gsize) 0x4, (gpointer) &signewnym_is_pending,
		(gsize) 0x4, (gpointer) &can_reach_or_port,
		(gsize) 0x8, (gpointer) &addressmap,
		(gsize) 0x4, (gpointer) &below_threshold,
		(gsize) 0x8, (gpointer) &current_ns_consensus,
		(gsize) 0x8, (gpointer) &global_state,
		(gsize) 0x4, (gpointer) &attr_reentrant,
		(gsize) 0x40, (gpointer) &voting_schedule,
		(gsize) 0x8, (gpointer) &ol_list,
		(gsize) 0x8, (gpointer) &stats_prev_n_written,
		(gsize) 0x58, (gpointer) &pk_op_counts,
		(gsize) 0x4, (gpointer) &fast_bandwidth,
		(gsize) 0x4, (gpointer) &n_circuits_allocated,
		(gsize) 0x4, (gpointer) &syslog_count,
		(gsize) 0x8, (gpointer) &guard_wfu,
		(gsize) 0x8, (gpointer) &authdir_badexit_policy,
		(gsize) 0x8, (gpointer) &desc_clean_since,
		(gsize) 0x8, (gpointer) &last_interface_ipv4,
		(gsize) 0x18, (gpointer) &last_warned,
		(gsize) 0x8, (gpointer) &fingerprint_list,
		(gsize) 0x8, (gpointer) &the_evdns_base,
		(gsize) 0x8, (gpointer) &predicted_internal_capacity_time,
		(gsize) 0x4, (gpointer) &global_read_bucket,
		(gsize) 0x4, (gpointer) &n_seconds_active_in_interval,
		(gsize) 0x8, (gpointer) &active_linked_connection_lst,
		(gsize) 0x4, (gpointer) &write_buckets_empty_last_second,
		(gsize) 0x8, (gpointer) &write_array,
		(gsize) 0x8, (gpointer) &freelist,
		(gsize) 0x18, (gpointer) &cache_root,
		(gsize) 0x4, (gpointer) &have_already_switched_id,
		(gsize) 0x4, (gpointer) &num_cpus,
		(gsize) 0x70, (gpointer) &cached_runningrouters,
		(gsize) 0x8, (gpointer) &authority_key_certificate,
		(gsize) 0x8, (gpointer) &time_to_check_port_forwarding,
		(gsize) 0xff8, (gpointer) &circ_times,
		(gsize) 0x4, (gpointer) &use_unsafe_renegotiation_flag,
		(gsize) 0x8, (gpointer) &dir_write_array,
		(gsize) 0x8, (gpointer) &lastonionkey,
		(gsize) 0x8, (gpointer) &dir_policy,
		(gsize) 0x8, (gpointer) &broken_connection_counts,
		(gsize) 0x4, (gpointer) &log_time_granularity,
		(gsize) 0x8, (gpointer) &start_of_buffer_stats_interval,
		(gsize) 0x68, (gpointer) &dirreqmap_PRIMES,
		(gsize) 0x8, (gpointer) &stats_n_seconds_working,
		(gsize) 0x8, (gpointer) &time_to_check_v3_certificate,
		(gsize) 0x20, (gpointer) &consensus_dl_status,
		(gsize) 0x4, (gpointer) &called_loop_once,
		(gsize) 0x8, (gpointer) &started_tracking_stability,
		(gsize) 0x8, (gpointer) &named_server_map,
		(gsize) 0x4, (gpointer) &last_state_file_write_failed,
		(gsize) 0x18, (gpointer) &early_warning_limit,
		(gsize) 0x4, (gpointer) &consider_republishing_rend_descriptors,
		(gsize) 0x8, (gpointer) &predicted_ports_list,
		(gsize) 0x4, (gpointer) &bootstrap_problems,
		(gsize) 0x8, (gpointer) &last_networkstatus_download_attempted,
		(gsize) 0x4, (gpointer) &cfg_start_day,
		(gsize) 0x8, (gpointer) &shutdown_time,
		(gsize) 0x18, (gpointer) &dirreq_map,
		(gsize) 0x68, (gpointer) &digestmap_impl_PRIMES,
		(gsize) 0x4, (gpointer) &stable_uptime,
		(gsize) 0x8, (gpointer) &_last_circid_orconn_ent,
		(gsize) 0x400, (gpointer) &last_sent_bootstrap_message,
		(gsize) 0x8, (gpointer) &key_lock,
		(gsize) 0x8, (gpointer) &server_identitykey,
		(gsize) 0x8, (gpointer) &dns_wildcarded_test_address_list,
		(gsize) 0x4, (gpointer) &warned_debugger_attach,
		(gsize) 0x8, (gpointer) &rend_cache,
		(gsize) 0x8, (gpointer) &start_of_entry_stats_interval,
		(gsize) 0x4, (gpointer) &cfg_start_hour,
		(gsize) 0x4, (gpointer) &_crypto_global_initialized,
		(gsize) 0x8, (gpointer) &authdir_baddir_policy,
		(gsize) 0x4, (gpointer) &global_event_mask,
		(gsize) 0x8, (gpointer) &trusted_dir_servers,
		(gsize) 0x8, (gpointer) &stats_n_create_cells_processed,
		(gsize) 0x8, (gpointer) &stats_n_authorize_cells_processed,
		(gsize) 0x4, (gpointer) &start_daemon_called,
		(gsize) 0x8, (gpointer) &pending_cb_messages,
		(gsize) 0x18, (gpointer) &ns_v2_responses,
		(gsize) 0x8, (gpointer) &stats_n_bytes_written,
		(gsize) 0x4, (gpointer) &trusted_dir_servers_certs_changed,
		(gsize) 0x4, (gpointer) &global_next_session_group,
		(gsize) 0x8, (gpointer) &geoip_entries,
		(gsize) 0x4, (gpointer) &guard_bandwidth_including_exits,
		(gsize) 0x8, (gpointer) &global_default_options,
		(gsize) 0x18, (gpointer) &disablenet_violated,
		(gsize) 0x8, (gpointer) &the_nodelist,
		(gsize) 0x8, (gpointer) &client_tls_context,
		(gsize) 0x8, (gpointer) &total_nonearly,
		(gsize) 0x8, (gpointer) &sl_last_total_weighted_bw,
		(gsize) 0x4, (gpointer) &hibernate_state,
		(gsize) 0x8, (gpointer) &legacy_signing_key,
		(gsize) 0x8, (gpointer) &lockfile,
		(gsize) 0x4, (gpointer) &dns_wildcard_one_notice_given,
		(gsize) 0x8, (gpointer) &n_bytes_at_soft_limit,
		(gsize) 0x4, (gpointer) &orconn_circid_map_N_PRIMES,
		(gsize) 0x71, (gpointer) &buf11,
		(gsize) 0x71, (gpointer) &buf10,
		(gsize) 0x4, (gpointer) &global_relayed_read_bucket,
		(gsize) 0x4, (gpointer) &can_reach_dir_port,
		(gsize) 0x10, (gpointer) &time_to_download_next_consensus,
		(gsize) 0x68, (gpointer) &strmap_impl_PRIMES,
		(gsize) 0x8, (gpointer) &start_of_exit_stats_interval,
		(gsize) 0x68, (gpointer) &cache_map_PRIMES,
		(gsize) 0x8, (gpointer) &guard_tk,
		(gsize) 0x8, (gpointer) &time_of_last_signewnym,
		(gsize) 0x8, (gpointer) &dh_param_p,
		(gsize) 0x4, (gpointer) &tls_library_is_initialized,
		(gsize) 0x8, (gpointer) &n_connections_allocated,
		(gsize) 0x8, (gpointer) &global_cmdline_options,
		(gsize) 0x8, (gpointer) &stats_n_versions_cells_processed,
		(gsize) 0x8, (gpointer) &dh_param_g,
		(gsize) 0x8, (gpointer) &warned_nicknames,
		(gsize) 0x1, (gpointer) &virtual_addr_netmask_bits,
		(gsize) 0x8, (gpointer) &time_to_run_helper,
		(gsize) 0x4, (gpointer) &have_warned_about_old_version,
		(gsize) 0x4, (gpointer) &backup_argc,
		(gsize) 0x8, (gpointer) &current_second,
		(gsize) 0x8, (gpointer) &dns_wildcard_response_count,
		(gsize) 0x8, (gpointer) &backup_argv,
		(gsize) 0x4, (gpointer) &server_is_advertised,
		(gsize) 0x4, (gpointer) &_n_openssl_mutexes,
		(gsize) 0x8, (gpointer) &info,
		(gsize) 0x8, (gpointer) &rend_cache_v2_dir,
		(gsize) 0x8, (gpointer) &previous_vote_list,
		(gsize) 0x8, (gpointer) &v2_download_status_map,
		(gsize) 0x8, (gpointer) &time_to_next_heartbeat,
		(gsize) 0x4, (gpointer) &use_unsafe_renegotiation_op,
		(gsize) 0x4, (gpointer) &finish_daemon_called,
		(gsize) 0x50, (gpointer) &first_good_0_2_1_guard_version,
		(gsize) 0x8, (gpointer) &authority_signing_key,
		(gsize) 0x8, (gpointer) &legacy_key_certificate,
		(gsize) 0x8, (gpointer) &geoip_countries,
		(gsize) 0x80, (gpointer) &dir_info_status,
		(gsize) 0x8, (gpointer) &global_dirfrontpagecontents,
		(gsize) 0x4, (gpointer) &freelist_len,
		(gsize) 0x4, (gpointer) &both_read_and_written,
		(gsize) 0x20, (gpointer) &buf4,
		(gsize) 0x8, (gpointer) &expected_bandwidth_usage,
		(gsize) 0x8, (gpointer) &country_idxplus1_by_lc_code,
		(gsize) 0x4, (gpointer) &need_to_update_have_min_dir_info,
		(gsize) 0x8, (gpointer) &socks_policy,
		(gsize) 0x100, (gpointer) &uname_result,
		(gsize) 0x8, (gpointer) &exit_bytes_written,
		(gsize) 0x8, (gpointer) &start_of_conn_stats_interval,
		(gsize) 0x18, (gpointer) &warning_limit1,
		(gsize) 0x8, (gpointer) &served_descs,
		(gsize) 0x8, (gpointer) &resolv_conf_mtime,
		(gsize) 0x20, (gpointer) &authentication_cookie,
		(gsize) 0x68, (gpointer) &bidimap_PRIMES,
		(gsize) 0x8, (gpointer) &stats_n_data_cells_received,
		(gsize) 0x8, (gpointer) &time_to_check_ipaddress

	);
}
