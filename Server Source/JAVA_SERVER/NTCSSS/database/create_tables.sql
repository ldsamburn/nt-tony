CREATE TABLE HOSTS (
hostname                TEXT     NOT NULL,
host_ip_address         TEXT     NOT NULL UNIQUE,
type                    INTEGER  NOT NULL,
replication_ind         BOOLEAN  NOT NULL,
max_running_jobs        INTEGER  NOT NULL,
CONSTRAINT hosts_pk PRIMARY KEY  (hostname)
);
 
CREATE TABLE USERS (
login_name              TEXT     NOT NULL,
long_login_name         TEXT     DEFAULT '_' NOT NULL,
real_name               TEXT     NOT NULL,
password                TEXT     NOT NULL,
unix_id                 INTEGER  NOT NULL,
ss_number               TEXT     NOT NULL,
phone_number            TEXT     NOT NULL,
security_class          INTEGER  NOT NULL,
pw_change_time          TEXT     NOT NULL,
login_lock              BOOLEAN  NOT NULL,
shared_db_passwd        TEXT,
auth_server             TEXT     NOT NULL,
editable_user           BOOLEAN  NOT NULL,
ntcss_super_user        BOOLEAN  NOT NULL,
 
CONSTRAINT users_pk PRIMARY KEY  (login_name),
CONSTRAINT hosts_fk FOREIGN KEY  (auth_server)
       REFERENCES HOSTS(hostname) ON DELETE CASCADE,
UNIQUE(long_login_name)
);

CREATE TABLE LOGIN_HISTORY (
login_name              TEXT    NOT NULL,
client_address          TEXT    NOT NULL,
login_time              TEXT    NOT NULL,
status                  INTEGER NOT NULL,
auth_server             TEXT
);

CREATE TABLE CURRENT_USERS (
login_name              TEXT     NOT NULL,
login_time              TEXT	 NOT NULL,
client_address          TEXT     NOT NULL,
token                   TEXT     NOT NULL UNIQUE,
reserver_time           TEXT	 NOT NULL,

CONSTRAINT current_users_pk    PRIMARY KEY (login_name),
CONSTRAINT current_users_ln_fk FOREIGN KEY (login_name)
       REFERENCES USERS(login_name) ON DELETE CASCADE
);

CREATE TABLE APPS (
app_title               TEXT     NOT NULL,
hostname                TEXT     NOT NULL,
app_lock                BOOLEAN  NOT NULL,
icon_file               TEXT,
icon_index              INTEGER  NOT NULL,
version_number          TEXT,
release_date            TEXT,
client_location         TEXT,
server_location         TEXT,
unix_group              TEXT     NOT NULL,
max_running_app_jobs    INTEGER  NOT NULL,
link_data               TEXT,
psp_filename            TEXT,
env_filename            TEXT,
termination_signal      INTEGER,
 
CONSTRAINT apps_pk    PRIMARY KEY (app_title),
CONSTRAINT apps_hn_fk FOREIGN KEY (hostname)
       REFERENCES HOSTS(hostname) ON DELETE CASCADE
);


CREATE TABLE APP_ROLES (
role_name               TEXT     NOT NULL,
app_title               TEXT     NOT NULL,
app_lock_override       BOOLEAN  NOT NULL,
role_number             INTEGER  NOT NULL,

CONSTRAINT app_roles_pk     PRIMARY KEY (role_name, app_title),
CONSTRAINT app_roles_at_fk  FOREIGN KEY (app_title)
       REFERENCES APPS(app_title) ON DELETE CASCADE
);


CREATE TABLE APP_USERS (
app_title              TEXT      NOT NULL,
login_name             TEXT      NOT NULL,
custom_app_data        TEXT,
app_passwd             TEXT,
registered_user        BOOLEAN   NOT NULL,
batch_user             BOOLEAN   NOT NULL,

CONSTRAINT app_users_pk     PRIMARY KEY  (app_title,login_name),
CONSTRAINT app_users_at_fk  FOREIGN KEY  (app_title)
       REFERENCES APPS(app_title) ON DELETE CASCADE,
CONSTRAINT app_users_ln_fk  FOREIGN KEY  (login_name)
       REFERENCES USERS(login_name) ON DELETE CASCADE
);


CREATE TABLE USER_APP_ROLES (
app_title               TEXT     NOT NULL,
login_name              TEXT     NOT NULL,
role_name               TEXT     NOT NULL,
 
CONSTRAINT user_app_roles_pk    PRIMARY KEY (app_title, login_name, role_name),
CONSTRAINT user_app_roles_atln_fk FOREIGN KEY (app_title, login_name)
       REFERENCES APP_USERS(app_title, login_name) ON DELETE CASCADE,
CONSTRAINT user_app_roles_rnat_fk FOREIGN KEY (role_name, app_title)
       REFERENCES APP_ROLES(role_name, app_title) ON DELETE CASCADE
);

CREATE TABLE APP_PROGRAMS (
program_title          TEXT     NOT NULL,
app_title              TEXT     NOT NULL,
program_file           TEXT     NOT NULL,
icon_file              TEXT,
icon_index             INTEGER  NOT NULL,
cmd_line_args          TEXT,
security_class         INTEGER  NOT NULL,
program_type           INTEGER,
working_dir            TEXT,

CONSTRAINT app_programs_pk  PRIMARY KEY (program_title, app_title),
CONSTRAINT app_programs_fk  FOREIGN KEY (app_title)
       REFERENCES APPS(app_title) ON DELETE CASCADE
);


CREATE TABLE APP_PROGRAM_ROLES (
role_name              TEXT     NOT NULL,
program_title          TEXT     NOT NULL,
app_title              TEXT     NOT NULL,

CONSTRAINT app_prog_roles_pk PRIMARY KEY (role_name, program_title, app_title),
CONSTRAINT app_prog_roles_ptat_fk FOREIGN KEY (program_title, app_title)
       REFERENCES APP_PROGRAMS(program_title, app_title) ON DELETE CASCADE,
CONSTRAINT app_prog_roles_rnat_fk FOREIGN KEY (app_title, role_name)
       REFERENCES APP_ROLES(app_title, role_name) ON DELETE CASCADE
);

CREATE TABLE PRINT_DRIVERS (
driver_name     TEXT     NOT NULL,
nt_monitor_name TEXT,

CONSTRAINT print_drivers_pk PRIMARY KEY (driver_name)
);

CREATE TABLE PRINT_DRIVER_FILES (
driver_name     TEXT     NOT NULL,
type            INTEGER,
driver_filename TEXT     NOT NULL,

CONSTRAINT print_driver_fls_pk PRIMARY KEY (driver_name, type, driver_filename),
CONSTRAINT driver_name_fk FOREIGN KEY (driver_name)
      REFERENCES PRINT_DRIVERS(driver_name) ON DELETE CASCADE
);

CREATE TABLE PRINTER_TYPES (
printer_type            TEXT     NOT NULL,
model                   TEXT     NOT NULL,
driver_name             TEXT     NOT NULL,
 
CONSTRAINT printer_types_pk PRIMARY KEY (printer_type)
);

CREATE TABLE ADAPTER_TYPES (
type                    TEXT     NOT NULL,
num_parallel_ports      INTEGER  NOT NULL,
num_serial_ports        INTEGER  NOT NULL,
setup_program           TEXT     NOT NULL,

CONSTRAINT adapter_types_pk     PRIMARY KEY (type)
);


CREATE TABLE PRINTER_ADAPTERS (
adapter_ip_address      TEXT     NOT NULL,
ethernet_address        TEXT     NOT NULL UNIQUE,
type                    TEXT     NOT NULL,
location                TEXT     NOT NULL,

CONSTRAINT printer_adapters_pk PRIMARY KEY (adapter_ip_address),
CONSTRAINT printer_adapters_fk FOREIGN KEY (type)
      REFERENCES ADAPTER_TYPES(type)
-- "ON DELETE CASCADE" left off for printer_adapters_fk on purpose
--  Do not want capability to remove a adapter_type and have it
--  cascade all the way down the database table structure
);


CREATE TABLE PRINTERS (
printer_name            TEXT     NOT NULL,
printing_method         TEXT     NOT NULL,
location                TEXT     NOT NULL,
job_max_bytes           INTEGER  NOT NULL,
printer_type            TEXT,
host_ip_address         TEXT,
security_class          INTEGER  NOT NULL,
queue_enabled           BOOLEAN  NOT NULL,
printer_enabled         BOOLEAN  NOT NULL,
general_access          BOOLEAN  NOT NULL,
port_name               TEXT,
suspend_requests        BOOLEAN  NOT NULL,
redirection_ptr         TEXT,
file_name               TEXT,
adapter_ip_address      TEXT,
file_append_flag        BOOLEAN,
nt_host_name            TEXT,
nt_share_name           TEXT,
driver_name             TEXT,
 
CONSTRAINT printers_pk PRIMARY KEY  (printer_name),
CONSTRAINT printers_pt_fk FOREIGN KEY  (printer_type)
       REFERENCES PRINTER_TYPES(printer_type) ON DELETE CASCADE,
CONSTRAINT printers_aip_fk FOREIGN KEY  (adapter_ip_address)
       REFERENCES PRINTER_ADAPTERS(adapter_ip_address) ON DELETE CASCADE,
CONSTRAINT printers_hip_fk FOREIGN KEY  (host_ip_address)
       REFERENCES HOSTS(host_ip_address) ON DELETE CASCADE,
CONSTRAINT printers_drv_fk FOREIGN KEY  (driver_name)
       REFERENCES PRINT_DRIVERS(driver_name) ON DELETE CASCADE
);


CREATE TABLE APP_PRINTERS (
printer_name            TEXT     NOT NULL,
app_title               TEXT     NOT NULL,

CONSTRAINT app_ptrs_pk           PRIMARY KEY (printer_name,app_title),
CONSTRAINT printer_access_pn_fk  FOREIGN KEY (printer_name)
       REFERENCES PRINTERS(printer_name) ON DELETE CASCADE,
CONSTRAINT printer_access_pt_fk  FOREIGN KEY (app_title)
       REFERENCES APPS(app_title) ON DELETE CASCADE
);


CREATE TABLE OUTPUT_TYPES (
otype_title             TEXT     NOT NULL,
app_title               TEXT     NOT NULL,
description             TEXT,
batch_flag              BOOLEAN  NOT NULL,
 
CONSTRAINT output_type_pk PRIMARY KEY     (otype_title, app_title),
CONSTRAINT output_type_app_fk FOREIGN KEY  (app_title)
       REFERENCES APPS(app_title) ON DELETE CASCADE
);


CREATE TABLE OUTPUT_PRINTERS (
otype_title             TEXT     NOT NULL,
app_title               TEXT     NOT NULL,
printer_name            TEXT     NOT NULL,
default_printer         BOOLEAN  NOT NULL,

CONSTRAINT output_ptrs_pk        PRIMARY KEY (otype_title, app_title, printer_name),
CONSTRAINT output_ptrs_otat_fk   FOREIGN KEY (otype_title, app_title)
       REFERENCES OUTPUT_TYPES (otype_title, app_title) ON DELETE CASCADE,
CONSTRAINT output_ptrs_atpn_fk   FOREIGN KEY (app_title, printer_name)
       REFERENCES APP_PRINTERS(app_title, printer_name) ON DELETE CASCADE
);


CREATE TABLE PREDEFINED_JOBS (
pdj_title               TEXT     NOT NULL,
app_title               TEXT     NOT NULL,
command_line            TEXT     NOT NULL,
job_type                INTEGER  NOT NULL,
schedule_str            TEXT,
 
CONSTRAINT predefined_jobs_pk    PRIMARY KEY (pdj_title, app_title),
CONSTRAINT predefined_jobs_at_fk FOREIGN KEY (app_title)
       REFERENCES APPS(app_title) ON DELETE CASCADE
);


CREATE TABLE PDJ_APP_ROLES (
pdj_title               TEXT     NOT NULL,
app_title               TEXT     NOT NULL,
role_name               TEXT     NOT NULL,

CONSTRAINT pdj_app_roles_pk    PRIMARY KEY  (pdj_title, app_title, role_name),
CONSTRAINT pdj_app_roles_ptat_fk FOREIGN KEY  (pdj_title, app_title)
       REFERENCES PREDEFINED_JOBS(pdj_title, app_title) ON DELETE CASCADE,
CONSTRAINT pdj_app_roles_rnat_fk FOREIGN KEY  (role_name, app_title)
       REFERENCES APP_ROLES(role_name, app_title) ON DELETE CASCADE
);


CREATE TABLE PRINT_REQUESTS (
request_id              TEXT     NOT NULL,
app_title               TEXT     NOT NULL,
filename                TEXT     NOT NULL,
login_name              TEXT,
printer_name            TEXT     NOT NULL,
time_of_request         TEXT	 NOT NULL,
security_class          INTEGER,
size                    INTEGER  NOT NULL,
orig_host               TEXT     NOT NULL,
status                  INTEGER,
CONSTRAINT prt_requests_pk PRIMARY KEY    (request_id),
CONSTRAINT prt_requests_at_fk FOREIGN KEY (app_title)
       REFERENCES APPS(app_title) ON DELETE CASCADE,
CONSTRAINT prt_requests_ln_fk FOREIGN KEY (login_name)
       REFERENCES USERS(login_name) ON DELETE CASCADE,
CONSTRAINT prt_requests_pn_fk FOREIGN KEY (printer_name)
       REFERENCES PRINTERS(printer_name) ON DELETE CASCADE
);


CREATE TABLE ACTIVE_JOBS (
ntcss_job_id            TEXT      NOT NULL,
command_line            TEXT      NOT NULL,
priority                INTEGER   NOT NULL,
security_class          INTEGER   NOT NULL,
schedule_string         TEXT,
job_initiator           TEXT,
job_requestor           TEXT      NOT NULL,
app_title               TEXT      NOT NULL,
job_description         TEXT,
request_time            TEXT,
launch_time             TEXT,
end_time                TEXT,
pid                     INTEGER,
exit_status             INTEGER,
originating_host        TEXT      NOT NULL,
restartable             BOOLEAN   NOT NULL,
custom_job_status       TEXT,

print_file_switch       BOOLEAN   NOT NULL,
printer_name            TEXT,
printer_sec_class       INTEGER,
print_filename          TEXT,
remove_print_file       BOOLEAN,
print_num_copies        INTEGER,
print_banner_switch     BOOLEAN,
print_page_size         INTEGER,
orient_portrait         BOOLEAN,

sdi_required            BOOLEAN   NOT NULL,
sdi_ctrl_rec            TEXT,
sdi_data_file           TEXT,
sdi_tci_file            TEXT,
sdi_device              TEXT,
sdi_read                BOOLEAN,
sdi_write               BOOLEAN,
sdi_failed              BOOLEAN,

CONSTRAINT active_procs_pk PRIMARY KEY    (ntcss_job_id),
-- The dependence on the USERS and PRINTERS table is removed so that 
--  when a user or printer is deleted, any cooresponding
--  records in this table will not be deleted. This would be
--  a problem when an active_job is running and its record is
--  removed from this table because its user or printer was
--  deleted. On the same note, we don't want a user or printer
--  delete to fail just because there is an active job running
--  for them.  The key to the APPS table remains because 
--  apps are only deleted during prime (when no jobs are running).
-- CONSTRAINT active_procs_ji_fk FOREIGN KEY (job_initiator)
--       REFERENCES USERS(login_name),
-- CONSTRAINT active_procs_jr_fk FOREIGN KEY (job_requestor)
--       REFERENCES USERS(login_name),
-- CONSTRAINT active_procs_pn_fk FOREIGN KEY (printer_name)
--       REFERENCES PRINTERS(printer_name),
CONSTRAINT active_procs_at_fk FOREIGN KEY (app_title)
       REFERENCES APPS(app_title)
);


CREATE TABLE SCHEDULED_JOBS (
ntcss_job_id            TEXT      NOT NULL,
command_line            TEXT      NOT NULL,
priority                INTEGER   NOT NULL,
security_class          INTEGER   NOT NULL,
schedule_string         TEXT      NOT NULL,
job_initiator           TEXT,
job_requestor           TEXT      NOT NULL,
app_title               TEXT      NOT NULL,
job_description         TEXT,
request_time            TEXT,
launch_time             TEXT,
end_time                TEXT,
pid                     INTEGER,
exit_status             INTEGER,
originating_host        TEXT      NOT NULL,
restartable             BOOLEAN   NOT NULL,
custom_job_status       TEXT,

print_file_switch       BOOLEAN   NOT NULL,
printer_name            TEXT,
printer_sec_class       INTEGER,
print_filename          TEXT,
remove_print_file       BOOLEAN,
print_num_copies        INTEGER,
print_banner_switch     BOOLEAN,
print_page_size         INTEGER,
orient_portrait         BOOLEAN,

sdi_required            BOOLEAN   NOT NULL,
sdi_ctrl_rec            TEXT,
sdi_data_file           TEXT,
sdi_tci_file            TEXT,
sdi_device              TEXT,
sdi_read                BOOLEAN,
sdi_write               BOOLEAN,
sdi_failed              BOOLEAN,

CONSTRAINT scheduled_jobs_pk PRIMARY KEY  (ntcss_job_id),
-- The dependence on the USERS and PRINTERS table is removed so that 
--  when a user or printer is deleted, any cooresponding
--  records in this table will not be deleted. This would be
--  a problem when an active_job is running and its record is
--  removed from this table because its user or printer was
--  deleted. On the same note, we don't want a user or printer
--  delete to fail just because there is an active job running
--  for them.  The key to the APPS table remains because 
--  apps are only deleted during prime (when no jobs are running).
-- CONSTRAINT scheduled_jobs_ji_fk FOREIGN KEY (job_initiator)
--       REFERENCES USERS(login_name),
-- CONSTRAINT scheduled_jobs_jr_fk FOREIGN KEY (job_requestor)
--       REFERENCES USERS(login_name),
-- CONSTRAINT scheduled_jobs_pn_fk FOREIGN KEY (printer_name)
--       REFERENCES PRINTERS(printer_name),
CONSTRAINT scheduled_jobs_at_fk FOREIGN KEY (app_title)
       REFERENCES APPS(app_title)
);


CREATE TABLE JOB_SCHEDULES (
ntcss_job_id            TEXT      NOT NULL,
valid_after_date        DATE      NOT NULL,
valid_after_time        TIME      NOT NULL, 
valid_until_date        DATE,
valid_until_time        TIME,
exclusion               BOOLEAN   NOT NULL,
frequency               TEXT      NOT NULL,
year                    INTEGER,
month                   INTEGER,
day_of_month            INTEGER,
day_of_week             INTEGER,
hour                    INTEGER,
minute                  INTEGER,

CONSTRAINT job_schedules_nji_fk FOREIGN KEY (ntcss_job_id)
       REFERENCES SCHEDULED_JOBS(ntcss_job_id) ON DELETE CASCADE
);


CREATE TABLE BULLETIN_BOARDS (
bb_name                 TEXT      NOT NULL,
creator                 TEXT      NOT NULL,

CONSTRAINT bul_brds_pk  PRIMARY KEY (bb_name)
);


CREATE TABLE BB_SUBSCRIBERS (
login_name              TEXT      NOT NULL,
bb_name                 TEXT      NOT NULL,

CONSTRAINT bb_subscribers_pk    PRIMARY KEY  (login_name, bb_name),
CONSTRAINT bb_subscribers_ln_fk FOREIGN KEY (login_name)
       REFERENCES USERS(login_name) ON DELETE CASCADE,
CONSTRAINT bb_subscribers_bb_fk FOREIGN KEY (bb_name)
       REFERENCES BULLETIN_BOARDS(bb_name) ON DELETE CASCADE
);


CREATE TABLE BB_MESSAGES (
msg_id                  TEXT      NOT NULL,
creation_time           TEXT	  NOT NULL,
login_name              TEXT      NOT NULL,
msg_title               TEXT      NOT NULL,
contents                TEXT      NOT NULL,
sent                    BOOLEAN   NOT NULL,

CONSTRAINT bb_messages_pk    PRIMARY KEY (msg_id),
CONSTRAINT bb_messages_ln_fk FOREIGN KEY (login_name)
       REFERENCES USERS(login_name) ON DELETE CASCADE
);


CREATE TABLE BB_READ_HISTORY (
msg_id                  TEXT      NOT NULL,
login_name              TEXT      NOT NULL,
isRead                  BOOLEAN   NOT NULL,

CONSTRAINT bb_read_hist_pk  PRIMARY KEY (msg_id, login_name),
CONSTRAINT bb_markers_mi_fk FOREIGN KEY (msg_id)
       REFERENCES BB_MESSAGES(msg_id) ON DELETE CASCADE,
CONSTRAINT bb_markers_ln_fk FOREIGN KEY (login_name)
       REFERENCES USERS(login_name) ON DELETE CASCADE
);


CREATE TABLE BB_MESSAGE_MAP (
bb_name                 TEXT      NOT NULL,
msg_id                  TEXT      NOT NULL,

CONSTRAINT bb_msg_map_pk    PRIMARY KEY (bb_name, msg_id),
CONSTRAINT bb_msg_map_bb_fk FOREIGN KEY (bb_name)
       REFERENCES BULLETIN_BOARDS(bb_name) ON DELETE CASCADE,
CONSTRAINT bb_msg_map_mi_fk FOREIGN KEY (msg_id)
       REFERENCES BB_MESSAGES(msg_id) ON DELETE CASCADE
);

CREATE TABLE SYS_CONF (
tag                 TEXT      NOT NULL,
value               TEXT,

CONSTRAINT sys_conf_pk    PRIMARY KEY (tag)
);






