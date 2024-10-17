CREATE DATABASE `accountdb` IF NOT EXIST 'accountDB';

CREATE TABLE IF NOT EXISTS `accountdb`.`account` (
	`accountno` BIGINT NOT NULL AUTO_INCREMENT,
	`userid` CHAR(64) NOT NULL,
	`userpass` CHAR(128) NOT NULL,
	`usernick` CHAR(64) NOT NULL,	
	PRIMARY KEY (`accountno`),
	INDEX `userid_INDEX` (`userid` ASC)
);

CREATE TABLE IF NOT EXISTS `accountdb`.`sessionkey` (
	`accountno` BIGINT NOT NULL,
	`sessionkey` CHAR(64) NULL,
    PRIMARY KEY (`accountno`)
);

CREATE TABLE IF NOT EXISTS `accountdb`.`status` (
	`accountno` BIGINT NOT NULL,
	`status` INT NOT NULL DEFAULT 0,
	PRIMARY KEY (`accountno`)
);






