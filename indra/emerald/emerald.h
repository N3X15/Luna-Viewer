/* Copyright (c) 2009
 *
 * Modular Systems. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MODULAR SYSTEMS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string>
#include <vector>

class EAscii85 {
private:
	EAscii85() { }
public:
	static std::string encode(const std::vector<unsigned char> &in);
	static std::vector<unsigned char> decode(const std::string &in);
};

class EAESEncrypt {
public:
	EAESEncrypt(const unsigned char *key, const unsigned char *iv);
	EAESEncrypt(const std::vector<unsigned char> &key, const std::vector<unsigned char> &iv);
	~EAESEncrypt();

	std::vector<unsigned char> encrypt(const std::string &in);

private:
	EAESEncrypt(const EAESEncrypt&) {}

	class EncryptImpl;

	EncryptImpl *mEncryptImpl;
};

class EAESDecrypt {
public:
	EAESDecrypt(const unsigned char *key, const unsigned char *iv);
	EAESDecrypt(const std::vector<unsigned char> &key, const std::vector<unsigned char> &iv);
	~EAESDecrypt();

	std::string decrypt(const std::vector<unsigned char> &in);

private:
	EAESDecrypt(const EAESDecrypt&) {}
	class DecryptImpl;

	DecryptImpl *mDecryptImpl;
};

class EGenKey {
public:
	EGenKey(const std::string &password, const unsigned char *salt = 0);
	~EGenKey();

	const unsigned char *key() const { return mKey; }
	const unsigned char *iv() const { return mIv; }

private:
	unsigned char mKey[16];
	unsigned char mIv[16];
};

class EDH {
public:
	EDH(const std::vector<unsigned char> &secret);
	EDH();
	~EDH();

	void secretTo(std::vector<unsigned char> &secret);
	void publicTo(std::vector<unsigned char> &pub);
	std::vector<unsigned char> computeKey(const std::vector<unsigned char> &other_pub);

private:
	EDH(const EDH&) {}

	class DHImpl;

	DHImpl *mDHImpl;
};

class EDSA {
public:
	EDSA(const std::vector<unsigned char> &secret);
	EDSA();
	~EDSA();

	std::vector<unsigned char> getPublic();
	void setPublic(const std::vector<unsigned char> &pub);
	std::vector<unsigned char> generateSecret();
	static bool verify(const std::vector<unsigned char> &dgst, const std::vector<unsigned char> &sig, const std::vector <unsigned char> &pub);
	bool verify(const std::vector<unsigned char> &dgst, const std::vector<unsigned char> &sig);
	std::vector<unsigned char> sign(const std::vector<unsigned char> &dgst);

private:
	EDSA(const EDSA&) {}

	class DSAImpl;

	DSAImpl *mDSAImpl;
};
