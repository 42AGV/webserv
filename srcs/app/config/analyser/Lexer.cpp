#include <Lexer.hpp>

Lexer::~Lexer(void) {
	delete tokens_;
}

static void addStringLit(std::list<std::string> *tokens, std::string *filebuff,
				  size_t *tokenend, size_t *line) {
	std::string token;
	char cmp;

	cmp = (*filebuff)[0];
	*filebuff = filebuff->substr(1);
	*tokenend = filebuff->find(cmp, 0);
	if (*tokenend == filebuff->npos) {
		delete tokens;
		throw Analyser::SyntaxError("Unterminated quote in line", *line);
	}
	token = filebuff->substr(0, *tokenend);
	tokens->push_back(token);
	*line += std::count(token.begin(), token.end(), '\n');
	(*tokenend)++;
}

static void addPunct(std::list<std::string> *tokens, char type,
					 size_t *tokenend) {
	char tmp[2];

	tmp[0] = type;
	tmp[1] = '\0';
	tokens->push_back(tmp);
	*tokenend = 1;
}

std::list<std::string> *Lexer::lexer(const std::string &fileBuff) {
	std::string filebuff = "{" + fileBuff + "}";  // add global scope
	std::list<std::string> *tokens = new std::list<std::string>;
	size_t line = 1;
	size_t tokenend = 0;
	std::string token;

	while (filebuff[0]) {
		filebuff = filebuff.substr(tokenend);
		for (; filebuff[0] && whitespace.find(filebuff[0], 0) != whitespace.npos
			 ; filebuff = filebuff.substr(1)) {
			if (filebuff[0] == '\n')
				line++;
		}
		if (filebuff[0] == '"' || filebuff[0] == '\'') {
			addStringLit(tokens, &filebuff, &tokenend, &line);
			continue;
		}
		if (validtokens.find(filebuff[0], 0) != validtokens.npos) {
			addPunct(tokens, filebuff[0], &tokenend);
			continue;
		}
		tokenend = filebuff.find_first_of(validtokens + whitespace, 0);
		if (tokenend == filebuff.npos)
			tokenend = filebuff.size();
		if ((token = filebuff.substr(0, tokenend)) != "")
			tokens->push_back(token);
	}
	return tokens;
}

std::string Lexer::preprocessor(std::ifstream &file) {
	std::string buffer;
	std::string filebuff;
	bool	insidedquote = false;
	bool	insidesquote = false;

	while(std::getline(file, buffer)) {
		size_t commentpos = buffer.npos;
		for(unsigned int i = 0 ; i < buffer.size(); ++i) {
			if (buffer[i] == '\'' && !insidedquote)
				insidesquote ^= true;
			else if (buffer[i] == '\"' && !insidesquote)
				insidedquote ^= true;
			else
				if (buffer[i] == '#' && !insidedquote && !insidesquote) {
					commentpos = i;
					break;
				}
		}
		if (commentpos == buffer.npos)
			filebuff += buffer;
		else
			filebuff += buffer.substr(0, commentpos);
		if (!file.eof())
			filebuff += "\n";
	}
	file.close();
	return filebuff;
}

std::list<std::string> *Lexer::GetTokens(void) const {
	return tokens_;
}

Lexer::Lexer(const std::string &path)
	: Analyser::Analyser(path),
	validtokens("{};"),
	  whitespace(" \t\f\n\r\t\v\n"),
	  tokens_(NULL) {
	std::ifstream	file(path_.c_str(), std::ios::binary);
	if (!file)
		throw std::invalid_argument(strerror(errno));
	filebuff_ = preprocessor(file);
	tokens_ = lexer(filebuff_);
}
