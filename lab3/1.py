#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
marker = 'iddqd'





class ErrorException(Exception):
    """Базовый класс для ошибок."""
    pass


def bin(s):
    """Переводит число в двоичную систему счисления.
    Результат в виде битовой строки.
    """
    return str(s) if s <= 1 else bin(s >> 1) + str(s & 1)


def byte2bin(bytes):
    """Поставляет байты в виде битовых строк.
    Преобразует символ в число.
    Переводит число в двоичную систему с дополнением нулей.
    """
    for b in bytes:
        yield bin(ord(b)).zfill(8)


def hide(bmp_filename, src_filename):
    """Помещает в BMP изображение любой файл, включая его название.
    У пикселей (байтов) в изображении меняет младшие биты на те,
    что содержатся в потоке бит скрываемого файла.
    Для определения в BMP секретных данных ставятся метки.
    """
    src = open(src_filename, 'rb')
    secret = marker + src_filename + marker + src.read() + marker
    src.close()

    bmp = open(bmp_filename, 'rb+')
    bmp.seek(55)
    container = bmp.read()

    need = 8 * len(secret) - len(container)
    if need > 0:
        raise ErrorException('BMP size is not sufficient for confidential file.'
                            '\nNeed another %s byte.' % need)
    cbits = byte2bin(container)
    encrypted = []
    for sbits in byte2bin(secret):
        for bit in sbits:
            bits = cbits.next()
            # Замена младшего бита в контейнерном байте
            bits = bits[:-1] + bit
            b = chr(int(bits, 2))
            # Замена байта в контейнере
            encrypted.append(b)

    bmp.seek(55)
    bmp.write(''.join(encrypted))
    bmp.close()

def decrypt_char(container):
    """Поставляет расшифрованные символы.
    Извлекает из 8 байт младшие биты, формирует из них битовую строку.
    Переводит из двоичной системы в десятичную.
    Преобразует число в символ.
    """
    sbits = ''
    for cbits in byte2bin(container):
        sbits += cbits[-1]
        if len(sbits) == 8:
            yield chr(int(sbits, 2))
            sbits = ''


def extract(bmp_filename):
    """Извлекает из BMP скрытый файл, включая его название."""
    bmp = open(bmp_filename, 'rb')
    bmp.seek(55)
    container = bmp.read()
    bmp.close()

    decrypted = []
    for b in decrypt_char(container):
        decrypted.append(b)
        # Определение, что в заданном изображении есть файл
        if (len(marker) == len(decrypted) and
            marker != ''.join(decrypted)):
            raise ErrorException('The image does not contain '
                                   'confidential file.')
    if len(decrypted) > len(marker):
        # Список ['', 'source file name', 'source file data', '']
        decrypted = ''.join(decrypted).split(marker)
        src_filename = decrypted[1]
        src_data = decrypted[2]
        src = open(src_filename, 'wb')
        src.write(src_data)
        src.close()

import argparse
parser=argparse.ArgumentParser(description='lab3')
        
def main():
    parser.add_argument('-i',type=str, help='Input file name',required=True)
    parser.add_argument('-o',type=str,help='File name you want to hide')
    parser.add_argument('-e',action="store_true",help='Encryption',default = False)
    parser.add_argument('-d',action="store_true",help='Decryption',default = False)
    args = parser.parse_args()
    if args.e and len(av)!=6:
    	print 'Usage 1.py [input file name] [hide file name] -e'
    if args.d and len(av)!=4:
    	print 'Usage 1.py [input file name with hide file] -d'
    if args.e==False and args.d==False:
		print 'Usage -e or -d'
		return 1
    if args.e and len(av) == 6:
		hide(format(args.i),format(args.o))
		return 1
    if args.d and len(av) == 4:
		extract(format(args.i))
		return 1


if __name__ == '__main__':
    from sys import argv as av
    main()
