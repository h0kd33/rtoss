23:00 2004/08/05
suga@snpn.net

repng2jpeg 1.0.4

jpeg,png,gif => jpeg,png,gif �̃t�H�[�}�b�g����уT�C�Y�ύX

����܂�g�����������Ǝv���Ă����̂ł���
����Ȃ�Ɏg���l��������ۂ��̂ł܂Ƃ߂Ă����Ă����܂�

�������Ă���o�C�i���̓R���p�C�����ɋ߂����
���̂܂ܓ����\��������Ƃ������܂����x�Ȃ̂�
�����Ȃ������炠����߂邱�Ƃ��K�v�ł�(��

���񑩎��ł������̃A�[�J�C�u�Ɋ܂܂�Ă���
�o�C�i��/�\�[�X�͊e���̐ӔC�ɂ����Ă����p������

OS�ɂ���Ă������p�ӂ��Ă݂܂���
dynamic   : libjpeg libpng �̋��L���C�u������K�v�Ƃ������ �t�@�C���T�C�Y��
standard  : libjpeg libpng ��static�ɂ��Ă������ libc������̃o�[�W�����������Ȃ瓮���͂�
static    : �X�^�e�B�b�N�o�C�i��  ex.linux�ł�linux�J�[�l���������Ă���Γ�������

���ɍs���قǎg�����L���C�u���������Ȃ��̂œ����\���������ł��A
���̂����t�@�C���T�C�Y���傫���Ȃ�܂��B

---
�R�}���h�̎g����

$repng2jpeg inputfile outputfile (width height|Z ratio) [Quality|P|G]

inputfile   -> ���̓t�@�C���� (JPEG,PNG,GIF)
outputfile  -> �o�̓t�@�C���� (JPEG,PNG,GIF)

width       -> ���T�C�Y��̉�px
height      -> ���T�C�Y��̏cpx

�c�����ۑ������܂܊g��k������ꍇ��
��O������ z �܂��� Z ���w�肵
��l������ �{�����w�肵�܂�

[option]
JPEG�������݂̏ꍇ JPEG�N�I���e�B�l(1-100)��
PNG�������݂̏ꍇ P ���� p ,
GIF�������݂̏ꍇ G ���� g ���w�肵�܂�
�ȗ����ꂽ�ꍇ�� 75 �������⊮����܂�

ex.
$repng2jpeg 1.jpg 2.jpg 400 300    # 400x300�Ƀ��T�C�Y��JPEG(75)�ŏ�������
$repng2jpeg 1.jpg 2.jpg 400 300 90 # 400x300�Ƀ��T�C�Y��JPEG(90)�ŏ�������
$repng2jpeg 1.jpg 2.png 400 300 P  # 400x300�Ƀ��T�C�Y��PNG�ŏ�������
$repng2jpeg 1.jpg 2.gif Z 0.5 G    # 0.5�{�Ƀ��T�C�Y��GIF�ŏ�������

---
�ˑ��֌W�Ƃ������z�z�Ńo�C�i���̎g�p���郉�C�u�����Q
static�ł͓��ɋ��L���C�u������K�v�Ƃ��܂���B

i386_freebsd4_dynamic
libjpeg -> libjpeg.so.9
libpng  -> libpng.so.5
libz    -> libz.so.2
libm    -> libm.so.2
libc    -> libc.so.4

i386_freebsd4_standard
libz    -> libz.so.2
libm    -> libm.so.2
libc    -> libc.so.4

i386_linux_dynamic
libjpeg -> libjpeg.so.62 
libpng  -> libpng.so.2
libz    -> libz.so.1
libm    -> libm.so.6
libc    -> libc.so.6
ld-linux.so.2

i386_linux_standard
libz    -> libz.so.1
libm    -> libm.so.6
libc    -> libc.so.6
ld-linux.so.2

i386_solaris_standard
libc    -> libc.so.1
libdl   -> libdl.so.1

sparc_solaris_standard
libc    -> libc.so.1
libdl   -> libdl.so.1
libc_psr.so

i386_win32
 -> msvcrt.dll

--
i386_freebsd4_dynamic    SIZE: 59036�@/ MD5: a375f3fc944740ca5706f0ea272fb5c0
i386_freebsd4_standard   SIZE: 254296 / MD5: ee64f97139cdce1222ec41aa79e32613
i386_freebsd4_static     SIZE: 371324 / MD5: 0ff9a08698c08b3898a50f6855cf1ab5
i386_linux_dynamic       SIZE: 59556  / MD5: 6f03c0ec02237299cd8f9a9e2212195a
i386_linux_standard      SIZE: 252468 / MD5: bf4db47cf12f43939892444da0d1ec98
i386_linux_static        SIZE: 661768 / MD5: 63e917c291e4476b0719756dd2df595b
i386_solaris_standard    SIZE: 332272 / MD5: 34091fe828658575ef5bdf9e21949816
i386_solaris_static      SIZE: 501804 / MD5: 0647c706ab30dc16779e71b3c7b7ae87
sparc_solaris_standard   SIZE: 416324 / MD5: d43ce38b0033ff474ec28c9df159c881
sparc_solaris_static     SIZE: 643624 / MD5: 27c0e47a8a439126212411d3c8fd0785
i386_win32               SIZE: 153600 / MD5: 043b3c41dd749365b960f1a7320a69ed


--
�R���p�C����
Linux   -> Vinelinux2.6r4(i386)
FreeBSD -> FreeBSD4.10R(i386)
Solaris -> SunOS5.8(i386/sparc)
Windows -> Windows2000SP4 + MinGW

---
1.0.3 -> 1.0.4 GIF�������݁A�{���I�v�V�����ǉ�
1.0.2 -> 1.0.3 GIF�ǂݍ��݃T�|�[�g
1.0.1 -> 1.0.2 PNG�������݃T�|�[�g
1.0.0 -> 1.0.1 JPEG�ǂݍ��݃T�|�[�g
1.0.0 png��jpeg�̃T�C�Y�y�уt�H�[�}�b�g�R���o�[�^�Ƃ��č쐬
