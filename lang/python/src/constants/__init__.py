# Constants.
#
# Copyright (C) 2016 g10 Code GmbH
#
# This file is part of GPGME.
#
# GPGME is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# GPGME is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
# Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, see <https://www.gnu.org/licenses/>.

from __future__ import absolute_import, print_function, unicode_literals

from gpg import util
# Globals may need to be set prior to module import, if so this prevents PEP8
# compliance, but better that than code breakage.
util.process_constants('GPGME_', globals())

# For convenience, we import the modules here.
from . import data, keylist, sig, tofu  # The subdirs.
# The remaining modules can no longer fit on one line.
from . import create, event, import_type, keysign, md, pk, protocol, sigsum
from . import status, validity

del absolute_import, print_function, unicode_literals, util

# This was a bad idea (though I get why it was done):
#
# # A complication arises because 'import' is a reserved keyword.
# # Import it as 'Import' instead.
# globals()['Import'] = getattr(
#     __import__('', globals(), locals(), [str('import')], 1), "import")

__all__ = [
    'data', 'event', 'import_type', 'keysign', 'keylist', 'md', 'pk',
    'protocol', 'sig', 'sigsum', 'status', 'tofu', 'validity', 'create'
]

# GPGME 1.7 replaced gpgme_op_edit with gpgme_op_interact.  We
# implement gpg.Context.op_edit using gpgme_op_interact, so the
# callbacks will be called with string keywords instead of numeric
# status messages.  Code that is using these constants will continue
# to work.

STATUS_ABORT = "ABORT"
STATUS_ALREADY_SIGNED = "ALREADY_SIGNED"
STATUS_ATTRIBUTE = "ATTRIBUTE"
STATUS_BACKUP_KEY_CREATED = "BACKUP_KEY_CREATED"
STATUS_BAD_PASSPHRASE = "BAD_PASSPHRASE"
STATUS_BADARMOR = "BADARMOR"
STATUS_BADMDC = "BADMDC"
STATUS_BADSIG = "BADSIG"
STATUS_BEGIN_DECRYPTION = "BEGIN_DECRYPTION"
STATUS_BEGIN_ENCRYPTION = "BEGIN_ENCRYPTION"
STATUS_BEGIN_SIGNING = "BEGIN_SIGNING"
STATUS_BEGIN_STREAM = "BEGIN_STREAM"
STATUS_CARDCTRL = "CARDCTRL"
STATUS_DECRYPTION_FAILED = "DECRYPTION_FAILED"
STATUS_DECRYPTION_INFO = "DECRYPTION_INFO"
STATUS_DECRYPTION_OKAY = "DECRYPTION_OKAY"
STATUS_DELETE_PROBLEM = "DELETE_PROBLEM"
STATUS_ENC_TO = "ENC_TO"
STATUS_END_DECRYPTION = "END_DECRYPTION"
STATUS_END_ENCRYPTION = "END_ENCRYPTION"
STATUS_END_STREAM = "END_STREAM"
STATUS_ENTER = "ENTER"
STATUS_ERRMDC = "ERRMDC"
STATUS_ERROR = "ERROR"
STATUS_ERRSIG = "ERRSIG"
STATUS_EXPKEYSIG = "EXPKEYSIG"
STATUS_EXPSIG = "EXPSIG"
STATUS_FAILURE = "FAILURE"
STATUS_FILE_DONE = "FILE_DONE"
STATUS_FILE_ERROR = "FILE_ERROR"
STATUS_FILE_START = "FILE_START"
STATUS_GET_BOOL = "GET_BOOL"
STATUS_GET_HIDDEN = "GET_HIDDEN"
STATUS_GET_LINE = "GET_LINE"
STATUS_GOOD_PASSPHRASE = "GOOD_PASSPHRASE"
STATUS_GOODMDC = "GOODMDC"
STATUS_GOODSIG = "GOODSIG"
STATUS_GOT_IT = "GOT_IT"
STATUS_IMPORT_OK = "IMPORT_OK"
STATUS_IMPORT_PROBLEM = "IMPORT_PROBLEM"
STATUS_IMPORT_RES = "IMPORT_RES"
STATUS_IMPORTED = "IMPORTED"
STATUS_INQUIRE_MAXLEN = "INQUIRE_MAXLEN"
STATUS_INV_RECP = "INV_RECP"
STATUS_INV_SGNR = "INV_SGNR"
STATUS_KEY_CONSIDERED = "KEY_CONSIDERED"
STATUS_KEY_CREATED = "KEY_CREATED"
STATUS_KEY_NOT_CREATED = "KEY_NOT_CREATED"
STATUS_KEYEXPIRED = "KEYEXPIRED"
STATUS_KEYREVOKED = "KEYREVOKED"
STATUS_LEAVE = "LEAVE"
STATUS_MISSING_PASSPHRASE = "MISSING_PASSPHRASE"
STATUS_MOUNTPOINT = "MOUNTPOINT"
STATUS_NEED_PASSPHRASE = "NEED_PASSPHRASE"
STATUS_NEED_PASSPHRASE_PIN = "NEED_PASSPHRASE_PIN"
STATUS_NEED_PASSPHRASE_SYM = "NEED_PASSPHRASE_SYM"
STATUS_NEWSIG = "NEWSIG"
STATUS_NO_PUBKEY = "NO_PUBKEY"
STATUS_NO_RECP = "NO_RECP"
STATUS_NO_SECKEY = "NO_SECKEY"
STATUS_NO_SGNR = "NO_SGNR"
STATUS_NODATA = "NODATA"
STATUS_NOTATION_DATA = "NOTATION_DATA"
STATUS_NOTATION_FLAGS = "NOTATION_FLAGS"
STATUS_NOTATION_NAME = "NOTATION_NAME"
STATUS_PINENTRY_LAUNCHED = "PINENTRY_LAUNCHED"
STATUS_PKA_TRUST_BAD = "PKA_TRUST_BAD"
STATUS_PKA_TRUST_GOOD = "PKA_TRUST_GOOD"
STATUS_PLAINTEXT = "PLAINTEXT"
STATUS_PLAINTEXT_LENGTH = "PLAINTEXT_LENGTH"
STATUS_POLICY_URL = "POLICY_URL"
STATUS_PROGRESS = "PROGRESS"
STATUS_REVKEYSIG = "REVKEYSIG"
STATUS_RSA_OR_IDEA = "RSA_OR_IDEA"
STATUS_SC_OP_FAILURE = "SC_OP_FAILURE"
STATUS_SC_OP_SUCCESS = "SC_OP_SUCCESS"
STATUS_SESSION_KEY = "SESSION_KEY"
STATUS_SHM_GET = "SHM_GET"
STATUS_SHM_GET_BOOL = "SHM_GET_BOOL"
STATUS_SHM_GET_HIDDEN = "SHM_GET_HIDDEN"
STATUS_SHM_INFO = "SHM_INFO"
STATUS_SIG_CREATED = "SIG_CREATED"
STATUS_SIG_ID = "SIG_ID"
STATUS_SIG_SUBPACKET = "SIG_SUBPACKET"
STATUS_SIGEXPIRED = "SIGEXPIRED"
STATUS_SUCCESS = "SUCCESS"
STATUS_TOFU_STATS = "TOFU_STATS"
STATUS_TOFU_STATS_LONG = "TOFU_STATS_LONG"
STATUS_TOFU_USER = "TOFU_USER"
STATUS_TRUNCATED = "TRUNCATED"
STATUS_TRUST_FULLY = "TRUST_FULLY"
STATUS_TRUST_MARGINAL = "TRUST_MARGINAL"
STATUS_TRUST_NEVER = "TRUST_NEVER"
STATUS_TRUST_ULTIMATE = "TRUST_ULTIMATE"
STATUS_TRUST_UNDEFINED = "TRUST_UNDEFINED"
STATUS_UNEXPECTED = "UNEXPECTED"
STATUS_USERID_HINT = "USERID_HINT"
STATUS_VALIDSIG = "VALIDSIG"
