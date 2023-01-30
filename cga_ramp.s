
[bits 16]
[org 0x100]

start:
        mov     ax,0002h
        int     10h

        mov     ax,0b800h
        mov     es,ax
        xor     di,di

        mov     bx,0f00h+'0'
loop1:
        xor     ax,ax
        stosw
        mov     ax,bx
        stosw
        xor     ax,ax
        stosw
        stosw
        stosw
        inc     bl
        cmp     bl,'9'+1
        jne     skip
        mov     bl,'a'
        skip:
        cmp     bl,'g'
        jne     loop1

        xor     bx,bx

y_loop:
x_loop:
        mov     al,0dbh                 ; CP 437 full block
        mov     ah,bl
        stosw
        stosw
        mov     ah,bh
        stosw
        stosw
        mov     al,bh
        add     al,'0'
        cmp     al,'9'
        jna     was_digit
        add     al,'a'-'0'-10
        was_digit:
        mov     ah,0fh
        stosw
        inc     bl
        cmp     bl,16
        jne     x_loop
        mov     bl,0
        inc     bh
        cmp     bh,16
        jne     y_loop

        xor     ax,ax
        int     16h

        mov     ax,0003h
        int     10h

        mov     ax,4c00h
        int     21h
