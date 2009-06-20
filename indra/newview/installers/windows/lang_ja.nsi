; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Japanese.nlf"

; subtitle on license text caption
LangString LicenseSubTitleUpdate ${LANG_JAPANESE} " �A�b�v�f�[�g" 
LangString LicenseSubTitleSetup ${LANG_JAPANESE} " �Z�b�g�A�b�v" 

; description on license page
LangString LicenseDescUpdate ${LANG_JAPANESE} "���̃p�b�P�[�W�̓Z�J���h���C�t���o�[�W����${VERSION_LONG}.�ɃA�b�v�f�[�g���܂��B" 
LangString LicenseDescSetup ${LANG_JAPANESE} "���̃p�b�P�[�W�͂��Ȃ��̃R���s���[�^�ɃZ�J���h���C�t���C���X�g�[�����܂��B" 
LangString LicenseDescNext ${LANG_JAPANESE} "��" 

; installation directory text
LangString DirectoryChooseTitle ${LANG_JAPANESE} "�C���X�g�[���E�f�B���N�g��" 
LangString DirectoryChooseUpdate ${LANG_JAPANESE} "�A�b�v�f�[�g����Z�J���h���C�t�̃f�B���N�g����I�����Ă��������B:" 
LangString DirectoryChooseSetup ${LANG_JAPANESE} "�Z�J���h���C�t���C���X�g�[������f�B���N�g����I�����Ă��������B: " 

; CheckStartupParams message box
LangString CheckStartupParamsMB ${LANG_JAPANESE} "�v���O������'$INSTPROG'��������܂���B�T�C�����g�E�A�b�v�f�[�g�Ɏ��s���܂����B" 

; installation success dialog
LangString InstSuccesssQuestion ${LANG_JAPANESE} "�����ɃZ�J���h���C�t���J�n���܂����H " 

; remove old NSIS version
LangString RemoveOldNSISVersion ${LANG_JAPANESE} "�Â��o�[�W���������`�F�b�N���ł��c" 

; check windows version
LangString CheckWindowsVersionDP ${LANG_JAPANESE} "�E�B���h�E�Y�̃o�[�W���������`�F�b�N���ł�..." 
LangString CheckWindowsVersionMB ${LANG_JAPANESE} "�Z�J���h���C�t��Windows XP�AWindows 2000�AMac OS X�݂̂��T�|�[�g���Ă��܂��BWindows $R0���C���X�g�[�����鎖�́A�f�[�^�̏�����N���b�V���̌����ɂȂ�\��������܂��B�C���X�g�[���𑱂��܂����H" 

; checkifadministrator function (install)
LangString CheckAdministratorInstDP ${LANG_JAPANESE} "�C���X�g�[���̂��߂̌������`�F�b�N���ł�..." 
LangString CheckAdministratorInstMB ${LANG_JAPANESE} "�Z�J���h���C�t���C���X�g�[������ɂ͊Ǘ��Ҍ������K�v�ł��B"

; checkifadministrator function (uninstall)
LangString CheckAdministratorUnInstDP ${LANG_JAPANESE} "�A���C���X�g�[���̂��߂̌������`�F�b�N���ł�..." 
LangString CheckAdministratorUnInstMB ${LANG_JAPANESE} "�Z�J���h���C�t���A���C���X�g�[������ɂ͊Ǘ��Ҍ������K�v�ł��B" 

; checkifalreadycurrent
LangString CheckIfCurrentMB ${LANG_JAPANESE} "�Z�J���h���C�t${VERSION_LONG} �̓C���X�g�[���ς݂ł��B�ēx�C���X�g�[�����܂����H " 

; closesecondlife function (install)
LangString CloseSecondLifeInstDP ${LANG_JAPANESE} "�Z�J���h���C�t���I�����ł�..." 
LangString CloseSecondLifeInstMB ${LANG_JAPANESE} "�Z�J���h���C�t�̋N�����ɃC���X�g�[���͏o���܂���B�����ɃZ�J���h���C�t���I�����ăC���X�g�[�����J�n����ꍇ��OK�{�^���������Ă��������BCANCEL�������ƒ��~���܂��B"

; closesecondlife function (uninstall)
LangString CloseSecondLifeUnInstDP ${LANG_JAPANESE} "�Z�J���h���C�t���I�����ł�..." 
LangString CloseSecondLifeUnInstMB ${LANG_JAPANESE} "�Z�J���h���C�t�̋N�����ɃA���C���X�g�[���͏o���܂���B�����ɃZ�J���h���C�t���I�����ăA���C���X�g�[�����J�n����ꍇ��OK�{�^���������Ă��������BCANCEL�������ƒ��~���܂��B" 

; removecachefiles
LangString RemoveCacheFilesDP ${LANG_JAPANESE} " Documents and Settings �t�H���_�̃L���b�V���t�@�C�����f���[�g���ł��B" 

; delete program files
LangString DeleteProgramFilesMB ${LANG_JAPANESE} "�Z�J���h���C�t�̃f�B���N�g���ɂ́A�܂��t�@�C�����c����Ă��܂��B$\n$INSTDIR$\n�ɂ��Ȃ����쐬�A�܂��͈ړ��������t�@�C��������\��������܂��B�S�č폜���܂����H " 

; uninstall text
LangString UninstallTextMsg ${LANG_JAPANESE} "�Z�J���h���C�t${VERSION_LONG}���A���C���X�g�[�����܂��B"
