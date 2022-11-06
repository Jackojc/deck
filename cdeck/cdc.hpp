#ifndef CDC_HPP
#define CDC_HPP

/*
	Headers are not included here for compilation
	speed reasons. Better to name all of your
	includes in the source file rather than force
	the preprocessor to track down files and
	potentially include them multiple times.

	#include <utility>
	#include <algorithm>
	#include <memory>
	#include <vector>
	#include <list>
	#include <string>
	#include <unordered_set>
	#include <unordered_map>
	#include <iostream>
	#include <cstddef>
	#include <cstdint>
*/

// Macros
namespace cdc {
	#define CDC_STR_IMPL_(x) #x
	#define CDC_STR(x) CDC_STR_IMPL_(x)

	#define CDC_CAT_IMPL_(x, y) x##y
	#define CDC_CAT(x, y) CDC_CAT_IMPL_(x, y)

	#define CDC_VAR(x) CDC_CAT(var_, CDC_CAT(x, CDC_CAT(__LINE__, _)))

	#define CDC_TRACE "[" __FILE__ ":" CDC_STR(__LINE__) "] "
}

// I/O
namespace cdc {
	namespace detail {
		template <typename... Ts>
		inline std::ostream& print_impl(std::ostream& os, Ts&&... xs) {
			return ((os << std::forward<Ts>(xs)), ..., os);
		}

		template <typename... Ts>
		inline std::ostream& println_impl(std::ostream& os, Ts&&... xs) {
			return ((os << std::forward<Ts>(xs)), ..., (os << '\n'));
		}
	}

	template <typename T, typename... Ts>
	inline std::ostream& print(std::ostream& os, T&& x, Ts&&... xs) {
		return detail::print_impl(os, std::forward<T>(x), std::forward<Ts>(xs)...);
	}

	template <typename T, typename... Ts>
	inline std::ostream& println(std::ostream& os, T&& x, Ts&&... xs) {
		return detail::println_impl(os, std::forward<T>(x), std::forward<Ts>(xs)...);
	}
}

// Logging
namespace cdc {
	#define CDC_RESET "\x1b[0m"
	#define CDC_BOLD  "\x1b[1m"

	#define CDC_BLACK   "\x1b[30m"
	#define CDC_RED     "\x1b[31m"
	#define CDC_GREEN   "\x1b[32m"
	#define CDC_YELLOW  "\x1b[33m"
	#define CDC_BLUE    "\x1b[34m"
	#define CDC_MAGENTA "\x1b[35m"
	#define CDC_CYAN    "\x1b[36m"
	#define CDC_WHITE   "\x1b[37m"

	#ifndef NDEBUG
		namespace detail {
			template <typename T> inline decltype(auto) dbg_impl(
				const char* file,
				const char* line,
				const char* expr_s,
				T&& expr
			) {
				println(std::cerr,
					"[", file, ":", line, "] ", expr_s, " = ", std::forward<T>(expr)
				);

				return std::forward<T>(expr);
			}
		}

		#define CDC_DBG(expr) \
			(cdc::detail::dbg_impl( \
				__FILE__, CDC_STR(__LINE__), CDC_STR(expr), (expr) \
			))

		#define CDC_DBG_RUN(expr) \
			do { ((expr)); } while (0)
	#else
		#define CDC_DBG(expr) ((expr))
		#define CDC_DBG_RUN(expr) do {} while (0)
	#endif

	#define LOG_LEVELS \
		X(INF, CDC_RESET "[-]") \
		X(WRN, CDC_BLUE  "[*]") \
		X(ERR, CDC_RED   "[!]") \
		X(OK,  CDC_GREEN "[^]")

	#define X(a, b) a,
		enum class LogLevel: size_t { LOG_LEVELS };
	#undef X

	namespace detail {
		#define X(a, b) b,
			constexpr const char* LOG2STR[] = { LOG_LEVELS };
		#undef X

		constexpr const char* log2str(LogLevel x) {
			return LOG2STR[static_cast<size_t>(x)];
		}
	}

	inline std::ostream& operator<<(std::ostream& os, LogLevel x) {
		return print(os, detail::log2str(x));
	}

	#define CDC_LOG(...) \
		do { [CDC_VAR(fn_name) = __func__] (LogLevel CDC_VAR(x), auto&&... CDC_VAR(args)) { \
			CDC_DBG_RUN(( \
				cdc::print(std::cerr, CDC_VAR(x), " ", CDC_TRACE) \
			)); \
			CDC_DBG_RUN(( cdc::print(std::cerr, "`", CDC_VAR(fn_name), "`" CDC_RESET) )); \
			if constexpr(sizeof...(CDC_VAR(args)) > 0) { CDC_DBG_RUN( \
				(cdc::print(std::cerr, " ", std::forward<decltype(CDC_VAR(args))>(CDC_VAR(args))...)) \
			); } \
			CDC_DBG_RUN(( cdc::print(std::cerr, '\n') )); \
		} ( __VA_ARGS__ ); } while (0)
}

// Utilities
namespace cdc {
	template <typename T, typename... Ts>
	[[nodiscard]] constexpr bool any(T&& first, Ts&&... rest) {
		return ((std::forward<T>(first) or std::forward<Ts>(rest)) or ...);
	}

	template <typename T, typename... Ts>
	[[nodiscard]] constexpr bool all(T&& first, Ts&&... rest) {
		return ((std::forward<T>(first) and std::forward<Ts>(rest)) and ...);
	}

	template <typename T, typename... Ts>
	[[nodiscard]] constexpr bool none(T&& first, Ts&&... rest) {
		return ((not std::forward<T>(first) and not std::forward<Ts>(rest)) and ...);
	}

	template <typename T, typename... Ts>
	[[nodiscard]] constexpr bool cmp_all(T&& first, Ts&&... rest) {
		return ((std::forward<T>(first) == std::forward<Ts>(rest)) and ...);
	}

	template <typename T, typename... Ts>
	[[nodiscard]] constexpr bool cmp_any(T&& first, Ts&&... rest) {
		return ((std::forward<T>(first) == std::forward<Ts>(rest)) or ...);
	}

	template <typename T, typename... Ts>
	[[nodiscard]] constexpr bool cmp_none(T&& first, Ts&&... rest) {
		return ((std::forward<T>(first) != std::forward<Ts>(rest)) and ...);
	}

	namespace detail {
		constexpr size_t length(const char* str) {
			const char *ptr = str;

			while (*ptr)
				++ptr;

			return ptr - str;
		}
	}

	constexpr auto hash_bytes(const char* begin, const char* const end) {
		size_t offset_basis = 14'695'981'039'346'656'037u;
		size_t prime = 1'099'511'628'211u;

		size_t hash = offset_basis;

		while (begin != end) {
			hash = (hash ^ static_cast<size_t>(*begin)) * prime;
			begin++;
		}

		return hash;
	}
}

// View
namespace cdc {
	struct View {
		const char* begin = nullptr;
		const char* end   = nullptr;

		constexpr View() {}

		constexpr View(const char* begin_, const char* end_):
			begin(begin_), end(end_) {}

		constexpr View(const char* begin_, size_t length):
			begin(begin_), end(begin_ + length) {}

		constexpr View(const char* ptr):
			begin(ptr), end(ptr + detail::length(ptr)) {}
	};

	constexpr size_t length(View sv) {
		return
			((sv.end - sv.begin) * (sv.end > sv.begin)) +
			((sv.begin - sv.end) * (sv.begin > sv.end));
	}

	constexpr bool cmp(View lhs, View rhs) {
		if (lhs.begin == rhs.begin and lhs.end == rhs.end)
			return true;

		if (length(lhs) != length(rhs))
			return false;

		for (size_t i = 0; i < length(lhs); i++) {
			if (*(lhs.begin + i) != *(rhs.begin + i))
				return false;
		}

		return true;
	}

	constexpr bool begins_with(View sv, View begins) {
		for (size_t i = 0; i < length(begins); i++) {
			if (*(sv.begin + i) != *(begins.begin + i))
				return false;
		}

		return true;
	}

	constexpr bool operator==(View lhs, View rhs) {
		return cmp(lhs, rhs);
	}

	constexpr bool operator!=(View lhs, View rhs) {
		return not(lhs == rhs);
	}

	[[nodiscard]] constexpr bool empty(View sv) {
		return sv.begin == sv.end;
	}

	inline std::ostream& operator<<(std::ostream& os, View sv) {
		os.write(sv.begin, length(sv));
		return os;
	}
}

constexpr cdc::View operator""_sv(const char* str, size_t n) {
	return { str, str + n };
}

namespace std {
	template <> struct hash<cdc::View> {
		constexpr size_t operator()(cdc::View v) const {
			return cdc::hash_bytes(v.begin, v.end);
		}
	};
}

namespace cdc {
	[[nodiscard]] constexpr char chr(View sv) {
		return *sv.begin;
	}

	[[nodiscard]] constexpr View stretch(View lhs, View rhs) {
		return { lhs.begin, rhs.end };
	}

	[[nodiscard]] constexpr View next(View sv) {
		if (empty(sv)) return sv;
		return { sv.begin + 1, sv.end };
	}

	[[nodiscard]] constexpr View peek(View sv) {
		if (empty(sv)) return sv;
		return { sv.begin, sv.begin + 1 };
	}

	[[nodiscard]] constexpr View take(View& sv) {
		if (empty(sv)) return sv;
		const char* ptr = sv.begin;
		sv = next(sv);
		return { ptr, sv.begin };
	}

	template <typename F>
	[[nodiscard]] constexpr View take_while(View& sv, F&& fn) {
		View out { sv.begin, sv.begin };

		while (not empty(sv) and fn(peek(sv)))
			out = stretch(out, take(sv));

		return out;
	}
}

// Errors
namespace cdc {
	#define ERROR_KINDS \
		X(GENERIC,            "an error occurred") \
		X(UNREACHABLE,        "unreachable code") \
		X(NOT_IMPLEMENTED,    "not implemented") \
		X(UNKNOWN_CHAR,       "unknown character") \
		\
		X(EXPECT_FN,          "expected a function") \
		X(EXPECT_IDENT,       "expected an identifier") \
		X(EXPECT_INSTRUCTION, "expected an instruction") \
		X(EXPECT_QUOTE,       "expected a quote") \
		X(EXPECT_UNQUOTE,     "expected end of quote") \
		X(EXPECT_LET,         "expected a let binding") \
		X(EXPECT_MARK,        "expected a mark") \
		X(EXPECT_UNMARK,      "expected end of mark") \
		X(EXPECT_LITERAL,     "expected a literal") \
		\
		X(CLASH,              "conflicting definitions") \
		X(UNDEFINED,          "undefined")

	#define X(a, b) a,
		enum class ErrorKind: size_t { ERROR_KINDS };
	#undef X

	namespace detail {
		#define X(a, b) b,
			constexpr const char* ERROR_TO_STR[] = { ERROR_KINDS };
		#undef X

		constexpr const char* error_to_str(ErrorKind x) {
			return ERROR_TO_STR[static_cast<size_t>(x)];
		}
	}

	inline std::ostream& operator<<(std::ostream& os, ErrorKind x) {
		return print(os, detail::error_to_str(x));
	}

	struct Report {
		View sv;
		ErrorKind kind;
	};

	template <typename... Ts>
	[[noreturn]] constexpr void report(View sv, ErrorKind x) {
		throw Report { sv, x };
	}

	inline std::ostream& report_handler(std::ostream& os, Report x) {
		return empty(x.sv) ?
			println(os, "error => ", x.kind):
			println(os, "error: `", x.sv, "` => ", x.kind);
	}

	inline std::ostream& operator<<(std::ostream& os, Report x) {
		return report_handler(os, x);
	}
}

// Lexer
namespace cdc {
	constexpr bool is_visible(View sv) {
		char x = chr(sv);
		return x >= 33 and x <= 126;
	}

	constexpr bool is_control(View sv) {
		char x = chr(sv);
		return x >= 0 and x <= 31;
	}

	constexpr bool is_whitespace(View sv) {
		char x = chr(sv);
		return any(x >= 9 and x <= 13, x == ' ');
	}

	constexpr bool is_digit(View sv) {
		char x = chr(sv);
		return x >= '0' and x <= '9';
	}

	#define SYMBOL_KINDS \
		X(NONE,     "None") \
		X(TERM,     "EOF") \
		\
		X(BIND,     "Bind") \
		\
		X(PROGRAM,  "Program") \
		X(ADDR,     "Address") \
		X(IDENT,    "Identifier") \
		X(INT,      "Integer") \
		X(STR,      "String") \
		\
		X(FN,       "Function") \
		X(LET,      "Let") \
		X(MARK,     "Mark") \
		X(QUOTE,    "Quote") \
		\
		X(UNMARK,   "Unmark") \
		X(UNQUOTE,  "Unquote") \
		\
		X(END,      "End") \
		\
		X(CALL,     "Call") \
		X(GO,       "Go") \
		X(COUNT,    "Count") \
		X(WORD,     "Word") \
		X(BYTE,     "Byte") \
		X(CHOOSE,   "Choose") \
		\
		X(DEQ_PUSH, "Deque Push") \
		X(DEQ_POP,  "Deque Pop") \
		\
		X(POP,      "Pop") \
		X(GET,      "Get") \
		X(SET,      "Set") \
		\
		X(ADD,      "Add") \
		X(SUB,      "Sub") \
		X(MUL,      "Mul") \
		X(DIV,      "Div") \
		X(MOD,      "Mod") \
		X(SHL,      "Shift Left") \
		X(SHR,      "Shift Right") \
		\
		X(AND,      "And") \
		X(OR,       "Or") \
		X(XOR,      "Xor") \
		X(NOT,      "Not") \
		\
		X(CMP,      "Compare")

	#define X(a, b) a,
		enum class SymbolKind: size_t { SYMBOL_KINDS };
	#undef X

	namespace detail {
		#define X(a, b) b,
			constexpr const char* SYMBOL_TO_STR[] = { SYMBOL_KINDS };
		#undef X

		constexpr const char* symbol_to_str(SymbolKind x) {
			return detail::SYMBOL_TO_STR[static_cast<size_t>(x)];
		}
	}

	inline std::ostream& operator<<(std::ostream& os, SymbolKind x) {
		return print(os, detail::symbol_to_str(x));
	}

	struct Symbol {
		View sv;
		SymbolKind kind;

		constexpr Symbol(View sv_, SymbolKind kind_):
			sv(sv_), kind(kind_) {}
	};

	inline std::ostream& operator<<(std::ostream& os, Symbol x) {
		return print(os, '{', x.kind, ",'", x.sv, "'}");
	}

	constexpr bool operator==(Symbol lhs, Symbol rhs) {
		return lhs.kind == rhs.kind and cmp(lhs.sv, rhs.sv);
	}

	constexpr bool operator!=(Symbol lhs, Symbol rhs) {
		return not(lhs == rhs);
	}

	struct Lexer {
		View src;
		View sv;

		Symbol peek;
		Symbol prev;

		constexpr Lexer(View src_):
			src(src_), sv(src_),
			peek(cdc::peek(src_), SymbolKind::NONE),
			prev(cdc::peek(src_), SymbolKind::NONE) {}
	};

	[[nodiscard]] inline Symbol take(Lexer& lx) {
		View ws = take_while(lx.sv, is_whitespace);
		Symbol sym { peek(lx.sv), SymbolKind::NONE };

		if (empty(lx.sv))  // EOF
			sym.kind = SymbolKind::TERM;

		else if (sym.sv == "["_sv) { sym.kind = SymbolKind::MARK; lx.sv = next(lx.sv); }
		else if (sym.sv == "]"_sv) { sym.kind = SymbolKind::UNMARK; lx.sv = next(lx.sv); }

		else if (sym.sv == "{"_sv) { sym.kind = SymbolKind::QUOTE; lx.sv = next(lx.sv); }
		else if (sym.sv == "}"_sv) { sym.kind = SymbolKind::UNQUOTE; lx.sv = next(lx.sv); }

		else if (sym.sv == "\""_sv) {
			sym.kind = SymbolKind::STR;
			lx.sv = next(lx.sv);

			sym.sv = take_while(lx.sv, [] (View sv) {
				return sv != "\"";
			});

			lx.sv = next(lx.sv);
		}

		else if (is_digit(sym.sv)) {
			sym.kind = SymbolKind::INT;
			sym.sv = take_while(lx.sv, is_digit);
		}

		else if (is_visible(sym.sv)) {  // Identifiers.
			sym.kind = SymbolKind::IDENT;
			sym.sv = take_while(lx.sv, is_visible);

			if (begins_with(sym.sv, "#!"_sv)) {  // Comments.
				View comment = take_while(lx.sv, [] (View sv) {
					return sv != "\n"_sv;
				});

				lx.sv = next(lx.sv);
				return take(lx);
			}

			// Builtins.
			if      (sym.sv == "."_sv)    sym.kind = SymbolKind::GO;
			else if (sym.sv == "::"_sv)   sym.kind = SymbolKind::FN;
			else if (sym.sv == "let"_sv)  sym.kind = SymbolKind::LET;
			else if (sym.sv == "#"_sv)    sym.kind = SymbolKind::COUNT;
			else if (sym.sv == "word"_sv) sym.kind = SymbolKind::WORD;
			else if (sym.sv == "byte"_sv) sym.kind = SymbolKind::BYTE;
			else if (sym.sv == "?"_sv)    sym.kind = SymbolKind::CHOOSE;
			else if (sym.sv == ">|"_sv)   sym.kind = SymbolKind::DEQ_PUSH;
			else if (sym.sv == "|>"_sv)   sym.kind = SymbolKind::DEQ_POP;
			else if (sym.sv == "pop"_sv)  sym.kind = SymbolKind::POP;
			else if (sym.sv == "get"_sv)  sym.kind = SymbolKind::GET;
			else if (sym.sv == "set"_sv)  sym.kind = SymbolKind::SET;
			else if (sym.sv == "+"_sv)    sym.kind = SymbolKind::ADD;
			else if (sym.sv == "-"_sv)    sym.kind = SymbolKind::SUB;
			else if (sym.sv == "*"_sv)    sym.kind = SymbolKind::MUL;
			else if (sym.sv == "/"_sv)    sym.kind = SymbolKind::DIV;
			else if (sym.sv == "%"_sv)    sym.kind = SymbolKind::MOD;
			else if (sym.sv == "<<"_sv)   sym.kind = SymbolKind::SHL;
			else if (sym.sv == ">>"_sv)   sym.kind = SymbolKind::SHR;
			else if (sym.sv == "&"_sv)    sym.kind = SymbolKind::AND;
			else if (sym.sv == "|"_sv)    sym.kind = SymbolKind::OR;
			else if (sym.sv == "^"_sv)    sym.kind = SymbolKind::XOR;
			else if (sym.sv == "~"_sv)    sym.kind = SymbolKind::NOT;
			else if (sym.sv == "<=>"_sv)  sym.kind = SymbolKind::CMP;
		}

		else
			report(sym.sv, ErrorKind::UNKNOWN_CHAR);

		Symbol out = lx.peek;

		lx.prev = lx.peek;
		lx.peek = sym;

		return out;
	}

	constexpr decltype(auto) is(SymbolKind kind) {
		return [=] (Symbol other) { return kind == other.kind; };
	}

	template <typename F> constexpr void expect(Lexer& lx, F&& fn, ErrorKind x) {
		if (not fn(lx.peek))
			report(lx.peek.sv, x);
	}
}

// Parser
namespace cdc {
	constexpr bool is_literal(Symbol x) {
		return cmp_any(x.kind,
			SymbolKind::INT,
			SymbolKind::STR);
	}

	constexpr bool is_instruction(Symbol x) {
		return cmp_any(x.kind,
			SymbolKind::IDENT,
			SymbolKind::INT,
			SymbolKind::STR,
			SymbolKind::GO,
			SymbolKind::COUNT,
			SymbolKind::WORD,
			SymbolKind::BYTE,
			SymbolKind::LET,
			SymbolKind::MARK,
			SymbolKind::QUOTE,
			SymbolKind::CHOOSE,
			SymbolKind::DEQ_PUSH,
			SymbolKind::DEQ_POP,
			SymbolKind::POP,
			SymbolKind::GET,
			SymbolKind::SET,
			SymbolKind::ADD,
			SymbolKind::SUB,
			SymbolKind::MUL,
			SymbolKind::DIV,
			SymbolKind::MOD,
			SymbolKind::SHL,
			SymbolKind::SHR,
			SymbolKind::AND,
			SymbolKind::OR,
			SymbolKind::XOR,
			SymbolKind::NOT,
			SymbolKind::CMP);
	}

	inline std::vector<Symbol> program(Lexer&);
	inline std::vector<Symbol> function(Lexer&);
	inline std::vector<Symbol> instruction(Lexer&);
	inline std::vector<Symbol> workspace(Lexer&);
	inline std::vector<Symbol> let(Lexer&);
	inline std::vector<Symbol> quote(Lexer&);

	inline Symbol label(Lexer&);
	inline Symbol literal(Lexer&);


	inline Symbol literal(Lexer& lx) {
		CDC_LOG(LogLevel::INF);
		expect(lx, is_literal, ErrorKind::EXPECT_LITERAL);
		return take(lx);
	}

	inline std::vector<Symbol> quote(Lexer& lx) {
		CDC_LOG(LogLevel::INF);

		std::vector<Symbol> symbols;

		expect(lx, is(SymbolKind::QUOTE), ErrorKind::EXPECT_QUOTE);
		symbols.push_back(take(lx));

		while (is_instruction(lx.peek)) {
			std::vector<Symbol> instructions = instruction(lx);
			symbols.insert(symbols.end(), instructions.begin(), instructions.end());
		}

		expect(lx, is(SymbolKind::UNQUOTE), ErrorKind::EXPECT_UNQUOTE);
		Symbol unquote = take(lx);

		symbols.emplace_back(unquote.sv, SymbolKind::END);

		return symbols;
	}

	inline std::vector<Symbol> let(Lexer& lx) {
		CDC_LOG(LogLevel::INF);

		std::vector<Symbol> symbols;

		expect(lx, is(SymbolKind::LET), ErrorKind::EXPECT_LET);
		Symbol let = take(lx);

		expect(lx, is(SymbolKind::IDENT), ErrorKind::EXPECT_IDENT);
		Symbol ident = take(lx);

		symbols.emplace_back(ident.sv, SymbolKind::LET);

		while (is_instruction(lx.peek)) {
			std::vector<Symbol> instructions = instruction(lx);
			symbols.insert(symbols.end(), instructions.begin(), instructions.end());
		}

		symbols.emplace_back(ident.sv, SymbolKind::END);

		return symbols;
	}

	inline std::vector<Symbol> workspace(Lexer& lx) {
		CDC_LOG(LogLevel::INF);

		std::vector<Symbol> symbols;

		expect(lx, is(SymbolKind::MARK), ErrorKind::EXPECT_MARK);
		symbols.push_back(take(lx));

		while (is_instruction(lx.peek)) {
			std::vector<Symbol> instructions = instruction(lx);
			symbols.insert(symbols.end(), instructions.begin(), instructions.end());
		}

		expect(lx, is(SymbolKind::UNMARK), ErrorKind::EXPECT_UNMARK);
		Symbol unmark = take(lx);

		symbols.emplace_back(unmark.sv, SymbolKind::END);

		return symbols;
	}

	inline std::vector<Symbol> instruction(Lexer& lx) {
		expect(lx, is_instruction, ErrorKind::EXPECT_INSTRUCTION);

		std::vector<Symbol> symbols;

		// Literals.
		if (cmp_any(lx.peek.kind, SymbolKind::INT, SymbolKind::STR))
			symbols = { literal(lx) };

		// Nested.
		else if (lx.peek.kind == SymbolKind::LET)
			symbols = let(lx);

		else if (lx.peek.kind == SymbolKind::MARK)
			symbols = workspace(lx);

		else if (lx.peek.kind == SymbolKind::QUOTE)
			symbols = quote(lx);

		// Builtins.
		else {
			CDC_LOG(LogLevel::INF);
			symbols.push_back(take(lx));
		}

		return symbols;
	}

	inline std::vector<Symbol> function(Lexer& lx) {
		CDC_LOG(LogLevel::INF);

		std::vector<Symbol> symbols;

		while (is_instruction(lx.peek)) {
			std::vector<Symbol> instructions = instruction(lx);
			symbols.insert(symbols.end(), instructions.begin(), instructions.end());
		}

		expect(lx, is(SymbolKind::FN), ErrorKind::EXPECT_FN);
		Symbol fn = take(lx);

		expect(lx, is(SymbolKind::IDENT), ErrorKind::EXPECT_IDENT);
		Symbol ident = take(lx);

		symbols.emplace(symbols.begin(), ident.sv, SymbolKind::FN);
		symbols.emplace_back(ident.sv, SymbolKind::END);

		return symbols;
	}

	inline std::vector<Symbol> program(Lexer& lx) {
		CDC_LOG(LogLevel::INF);

		std::vector<Symbol> symbols;
		symbols.emplace_back(lx.peek.sv, SymbolKind::PROGRAM);

		while (lx.peek.kind != SymbolKind::TERM) {
			std::vector<Symbol> instructions = function(lx);
			symbols.insert(symbols.end(), instructions.begin(), instructions.end());
		}

		symbols.emplace_back(lx.peek.sv, SymbolKind::END);

		return symbols;
	}

	// Semantic analysis passes.
	struct Context {
		std::unordered_map<View, SymbolKind> symbols;
		std::unordered_map<View, std::unordered_set<View>> refs;
		std::unordered_map<View, std::pair<
			std::vector<Symbol>::iterator,
			std::vector<Symbol>::iterator
		>> ranges;
		std::list<std::string> intern;
		size_t intern_id = 0;
	};

	// Visit a block (i.e. a run of code terminated by `end`).
	// This is a common pattern that shows up in most visitors that
	// traverse the AST like a tree rather than a vector.
	template <typename F, typename... Ts>
	inline std::vector<Symbol>::iterator visit_block(
		Context& ctx,
		std::vector<Symbol>::iterator it,
		std::vector<Symbol>& instructions,
		F&& fn,
		Ts&&... args
	) {
		while (it->kind != SymbolKind::END)
			it = fn(ctx, it, instructions, std::forward<Ts>(args)...);

		return it + 1;
	}

	// Introduce a pass to re-order function definitions so that the definition
	// always appears before any calls. We can do this by first generating a call
	// graph (which in deck is incidentally also the control flow graph) and then
	// doing a topological sort to find the order in which the functions need to be
	// defined. Once we have the order we need, we can try iterating the AST and
	// shifting functions up to some cursor that we advance as we move along the
	// topologically sorted function list.

	// Stack effect checking pass to discover how many inputs and outputs functions
	// have. We can build up from primitives and then store the effects of user defined
	// functions and eventually have a stack effect for every function.

		// First build a call graph by iterating every function
		// and seeing which functions are called by it.
		// Add std::unordered_map<View, std::unordered_set<View>> member
		// to Context.
		// Next we have to a DFS on this callgraph to generate an ordering
		// for the functions. This is a topological sort.
		// Once we have the ordering we want we can iterate through the
		// instructions and perform what is basically a sort by swapping
		// functions if the ordering is mismatched relative to eachother.
}

#endif
