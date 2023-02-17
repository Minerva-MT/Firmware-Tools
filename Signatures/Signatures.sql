CREATE TABLE Signatures ( Name varchar(45), Description varchar(100), Signature BLOB );

INSERT INTO Signatures values ('PFS', 'Professional File System', X'5046532f302e39');
INSERT INTO Signatures values ('Belkin', 'Belkin Partition Identifier', X'78563412');
INSERT INTO Signatures values ('LZMA', 'LZMA Compressed Archive', X'5D00008000');
INSERT INTO Signatures values ('ELF', 'Executable and Linkable Format', X'7F454C46');
INSERT INTO Signatures values ('HTML Header', 'HTML <HTML> Tag', X'3C68746D6C3E');
INSERT INTO Signatures values ('HTML Footer', 'HTML </HTML> Tag', X'3C2F68746D6C3E');
INSERT INTO Signatures values ('XML', 'XML Header Tag', X'3C3F786D6C2076657273696F6E3D22312E30223F3E');

