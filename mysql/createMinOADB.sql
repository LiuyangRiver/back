create database if not exists minoa character set utf8mb4 collate utf8mb4_general_ci;
use minoa ;
drop trigger if exists user.user_insert_triger;
drop trigger if exists realtimeMsg.realtimeMsg_insert_triger;
drop trigger if exists approval.approval_update_triger;
drop trigger if exists approval.approval_insert_triger;
drop table if exists approval;
drop table if exists whitelist ;
drop table if exists realtimeMsg ;
drop table if exists inform ;
drop table if exists user ;
create table if not exists user(
	ID int unsigned auto_increment,
	Username varchar(100) not null ,
	Prefix varchar(1000) not null ,
	Level int not null ,
	Pubkey varchar(1000) not null ,
	Realname varchar(100) not null ,
	Phone varchar(20) not null ,
	IDCard varchar(20) not null ,
	Password varchar(40) ,
	AboutMe text ,
	Print text ,
	Face text ,
	Iris text ,
	Other text ,
	avatar varchar(100) not null default "share/avatar.png",   --  头像
	Gender int not null ,
	lastMsgTime bigint default 0  ,	
	lastMsgId int default 0  ,	
	lastApprovalTime bigint default 0,
	lastApprovalId int  default 0  ,
	lastInformTime bigint default 0,
	lastInformId int  default 0  ,
	primary key (ID), 
	unique usernameidx (Username(100)) , 
	unique phoneidx (Phone(20))  
)character set utf8mb4 collate utf8mb4_general_ci ;

DELIMITER //
create trigger user_insert_triger before insert on user 
for each row begin
	set new.lastMsgTime = unix_timestamp(current_timestamp) ;
	set new.lastApprovalTime = unix_timestamp(current_timestamp) ;
	set new.lastInformTime = unix_timestamp(current_timestamp) ;
end 
//
DELIMITER ;

create table if not exists realtimeMsg(
	ID int unsigned auto_increment,
	FromName varchar(100)  ,
	ToName varchar(100) ,
	Msg text ,
	time bigint ,
	status int , --   0用户未接收 ，
	primary key (ID), 
	foreign key (FromName) references user(Username) ,
	foreign key (ToName) references user(Username) 
)character set utf8mb4 collate utf8mb4_general_ci;
DELIMITER //
create trigger realtimeMsg_insert_triger before insert on realtimeMsg
for each row begin
	set new.time = unix_timestamp(current_timestamp) ;
end 
//
DELIMITER ;

create table if not exists approval(
	ID int unsigned auto_increment,
	proposer varchar(100) not null,
	approver varchar(100) not null default "",  --  审批人
	time bigint,
	typecode int ,
	status int not null default 0,	--   0 未读， 1 已读
	process int not null default 0,	--   0 等待审批， 1审批过程中 , 2审批完成
	detail text,
	approvalRst int default 0,			--  0不通过审批，1通过审批
	reason text ,
	primary key (ID), 
	foreign key (proposer) references user(Username)
	--  foreign key (approver) references user(Username) 
)character set utf8mb4 collate utf8mb4_general_ci;
DELIMITER //
create trigger approval_update_triger before update on approval
for each row begin
	set new.time = unix_timestamp(current_timestamp) ;
end 
//
create trigger approval_insert_triger before insert on approval
for each row begin
	set new.time = unix_timestamp(current_timestamp) ;
end 
//
DELIMITER ;


create table if not exists whitelist(
	Phone varchar(20) primary key ,
	Realname varchar(100) not null,
	Gender int not null default 0,
	IDCard varchar(20) not null 
)character set utf8mb4 collate utf8mb4_general_ci;

insert into whitelist (Phone,Realname,IDCard) values ("00000000001","韦国华","4500000000001") ;
insert into whitelist (Phone,Realname,IDCard) values ("00000000002","韦国华","4500000000002") ;
insert into whitelist (Phone,Realname,IDCard) values ("00000000003","韦国华","4500000000003") ;
insert into whitelist (Phone,Realname,IDCard) values ("00000000004","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286740","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286741","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286742","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286743","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286744","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286745","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286746","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286747","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286748","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("13670286749","韦国华","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857280","阙建明","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857281","阙建明","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857282","阙建明","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857283","阙建明","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857284","阙建明","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857285","阙建明","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857286","阙建明","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857287","阙建明","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857288","阙建明","4500000000004") ;
insert into whitelist (Phone,Realname,IDCard) values ("18340857289","阙建明","4500000000004") ;

insert into whitelist (Phone,Realname,IDCard) values ("10000000006","王锋","4500000000006") ;
insert into whitelist (Phone,Realname,IDCard) values ("10000000007","王锋","4500000000007") ;
insert into whitelist (Phone,Realname,IDCard) values ("10000000008","王锋","4500000000008") ;
insert into whitelist (Phone,Realname,IDCard) values ("10000000009","王锋","4500000000009") ;

