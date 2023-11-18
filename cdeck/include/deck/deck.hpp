#ifndef DECK_HPP
#define DECK_HPP

#include <cstddef>
#include <cstdint>

#include <memory>
#include <utility>
#include <algorithm>
#include <filesystem>

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <list>

#include <iostream>

// Macros
namespace deck {
#define DECK_STR_IMPL_(x) #x
#define DECK_STR(x)       DECK_STR_IMPL_(x)

#define DECK_CAT_IMPL_(x, y) x##y
#define DECK_CAT(x, y)       DECK_CAT_IMPL_(x, y)

#define DECK_VAR(x) DECK_CAT(var_, DECK_CAT(x, DECK_CAT(__LINE__, _)))

#define DECK_TRACE "[" __FILE__ ":" DECK_STR(__LINE__) "] "
}  // namespace deck

// I/O
namespace deck {
	template <typename... Ts>
	inline std::ostream& print(std::ostream& os, Ts&&... args) {
		return (os << ... << std::forward<Ts>(args));
	}

	template <typename... Ts>
	inline std::ostream& println(std::ostream& os, Ts&&... args) {
		return print(os, std::forward<Ts>(args)..., '\n');
	}
}  // namespace deck

// Logging
namespace deck {
#define DECK_RESET "\x1b[0m"
#define DECK_BOLD  "\x1b[1m"

#define DECK_BLACK   "\x1b[30m"
#define DECK_RED     "\x1b[31m"
#define DECK_GREEN   "\x1b[32m"
#define DECK_YELLOW  "\x1b[33m"
#define DECK_BLUE    "\x1b[34m"
#define DECK_MAGENTA "\x1b[35m"
#define DECK_CYAN    "\x1b[36m"
#define DECK_WHITE   "\x1b[37m"

#ifndef NDEBUG
	namespace detail {
		template <typename T>
		inline decltype(auto) dbg_impl(const char* file, const char* line, const char* expr_s, T&& expr) {
			println(std::cerr,
				"[",
				std::filesystem::relative(file).native(),
				":",
				line,
				"] ",
				expr_s,
				" = ",
				std::forward<T>(expr));

			return std::forward<T>(expr);
		}
	}  // namespace detail

#define DECK_DBG(expr) (deck::detail::dbg_impl(__FILE__, DECK_STR(__LINE__), DECK_STR(expr), (expr)))

#define DECK_DBG_RUN(expr) \
	do { \
		((expr)); \
	} while (0)

#else

#define DECK_DBG(expr) ((expr))
#define DECK_DBG_RUN(expr) \
	do { \
	} while (0)
#endif

#define LOG_LEVELS \
	X(Info, DECK_RESET "[-]") \
	X(Warn, DECK_BLUE "[*]") \
	X(Fail, DECK_RED "[!]") \
	X(Okay, DECK_GREEN "[^]")

#define X(a, b) a,
	enum class Priority : size_t {
		LOG_LEVELS
	};
#undef X

	namespace detail {
#define X(a, b) b,
		constexpr const char* LOG_TO_STR[] = { LOG_LEVELS };
#undef X

		constexpr const char* log_to_str(Priority x) {
			return LOG_TO_STR[static_cast<size_t>(x)];
		}
	}  // namespace detail

	inline std::ostream& operator<<(std::ostream& os, Priority x) {
		return print(os, detail::log_to_str(x));
	}

	// Errors/Warnings
	template <typename... Ts>
	inline void info(Ts&&... args) {
		println(std::cerr, detail::log_to_str(Priority::Info), " ", std::forward<Ts>(args)..., DECK_RESET);
	}

	template <typename... Ts>
	inline void warn(Ts&&... args) {
		println(std::cerr, detail::log_to_str(Priority::Warn), " ", std::forward<Ts>(args)..., DECK_RESET);
	}

	template <typename... Ts>
	inline void fail(Ts&&... args) {
		println(std::cerr, detail::log_to_str(Priority::Fail), " ", std::forward<Ts>(args)..., DECK_RESET);
	}

	template <typename... Ts>
	inline void okay(Ts&&... args) {
		println(std::cerr, detail::log_to_str(Priority::Okay), " ", std::forward<Ts>(args)..., DECK_RESET);
	}

#define DECK_LOG(...) \
	do { \
		[DECK_VAR(fn_name) = __func__]([[maybe_unused]] deck::Priority DECK_VAR(x), auto&&... DECK_VAR(args)) { \
			using namespace std::string_view_literals; \
			((deck::print(std::cerr, \
				DECK_VAR(x), \
				" [", \
				std::filesystem::relative(__FILE__).native(), \
				":", \
				DECK_STR(__LINE__), \
				"] "))); \
			if (DECK_VAR(fn_name) != "operator()"sv) { \
				((deck::print(std::cerr, "`", DECK_VAR(fn_name), "` "))); \
			} \
			if constexpr (sizeof...(DECK_VAR(args)) > 0) { \
				((deck::print(std::cerr, DECK_RESET, std::forward<decltype(DECK_VAR(args))>(DECK_VAR(args))...))); \
			} \
			((deck::print(std::cerr, '\n', DECK_RESET))); \
		}(__VA_ARGS__); \
	} while (0)

	// Exceptions
	class Exception: public std::runtime_error {
		using runtime_error::runtime_error;
	};

	template <typename... Ts>
	[[noreturn]] inline void fatal(Ts&&... args) {
		std::ostringstream ss;
		print(ss, detail::log_to_str(Priority::Fail), " ", std::forward<Ts>(args)..., DECK_RESET);
		throw Exception { ss.str() };
	}

#define DECK_ASSERT(cond) \
	do { \
		if (not(cond)) { \
			deck::fatal("assertion failed: `", #cond, "`!"); \
		} \
	} while (0)

}  // namespace deck

// Utilities
namespace deck {
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
	[[nodiscard]] constexpr bool eq_all(T&& first, Ts&&... rest) {
		return ((std::forward<T>(first) == std::forward<Ts>(rest)) and ...);
	}

	template <typename T, typename... Ts>
	[[nodiscard]] constexpr bool eq_any(T&& first, Ts&&... rest) {
		return ((std::forward<T>(first) == std::forward<Ts>(rest)) or ...);
	}

	template <typename T, typename... Ts>
	[[nodiscard]] constexpr bool eq_none(T&& first, Ts&&... rest) {
		return ((std::forward<T>(first) != std::forward<Ts>(rest)) and ...);
	}
}  // namespace deck

// Lexer
namespace deck {
	constexpr bool is_visible(const char* ptr) {
		char x = *ptr;
		return x >= 33 and x <= 126;
	}

	constexpr bool is_control(const char* ptr) {
		char x = *ptr;
		return x >= 0 and x <= 31;
	}

	constexpr bool is_whitespace(const char* ptr) {
		char x = *ptr;
		return any(x >= 9 and x <= 13, x == ' ');
	}

	constexpr bool is_digit(const char* ptr) {
		char x = *ptr;
		return x >= '0' and x <= '9';
	}

#define SYMBOL_KINDS \
	X(None, "None") \
	X(Terminator, "Terminator") \
\
	X(Intrinsic, "Intrinsic") \
	X(Declare, "Declare") \
	X(Address, "Address") \
	X(Label, "Label") \
\
	X(Identifier, "Identifier") \
	X(Integer, "Integer") \
	X(String, "String") \
	X(Character, "Character") \
\
	X(Frame, "Stack Frame") \
	X(FrameEnd, "Stack Frame End") \
\
	X(Quote, "Quote") \
	X(QuoteEnd, "QuoteEnd") \
\
	X(Header, "Header") \
	X(Footer, "Footer") \
\
	X(End, "End")

#define X(a, b) a,
	enum class SymbolKind : size_t {
		SYMBOL_KINDS
	};
#undef X

	namespace detail {
#define X(a, b) b,
		constexpr const char* SYMBOL_TO_STR[] = { SYMBOL_KINDS };
#undef X

		constexpr const char* symbol_to_str(SymbolKind x) {
			return detail::SYMBOL_TO_STR[static_cast<size_t>(x)];
		}
	}  // namespace detail

	inline std::ostream& operator<<(std::ostream& os, SymbolKind x) {
		return print(os, detail::symbol_to_str(x));
	}

	struct Symbol {
		std::string str;
		SymbolKind kind;

		Symbol(std::string str_, SymbolKind kind_): str(str_), kind(kind_) {}
	};

	inline std::ostream& operator<<(std::ostream& os, Symbol x) {
		return print(os, '{', x.kind, ",'", x.str, "'}");
	}

	inline bool operator==(Symbol lhs, Symbol rhs) {
		return lhs.kind == rhs.kind and lhs.str == rhs.str;
	}

	inline bool operator!=(Symbol lhs, Symbol rhs) {
		return not(lhs == rhs);
	}

	struct Lexer {
		std::string src;
		const char* ptr;

		Symbol peek;

		Lexer(std::string&& src_): src(std::move(src_)), ptr(src.data()), peek("", SymbolKind::None) {
			Symbol sym = take();
		}

		[[nodiscard]] inline Symbol take() {
			while (is_whitespace(ptr)) {
				++ptr;
			}

			const char* begin = ptr;
			size_t length = 0;

			SymbolKind kind = SymbolKind::None;

			if (*ptr == '\0') {  // EOF
				kind = SymbolKind::Terminator;
			}

			else if (*ptr == '[') {
				kind = SymbolKind::Frame;
				++ptr;
			}

			else if (*ptr == ']') {
				kind = SymbolKind::FrameEnd;
				++ptr;
			}

			else if (*ptr == '{') {
				kind = SymbolKind::Quote;
				++ptr;
			}

			else if (*ptr == '}') {
				kind = SymbolKind::QuoteEnd;
				++ptr;
			}

			else if (*ptr == '$') {
				kind = SymbolKind::Intrinsic;
				begin = ++ptr;

				while (is_visible(ptr)) {
					++ptr;
				}
			}

			else if (*ptr == '&') {
				kind = SymbolKind::Address;
				begin = ++ptr;

				while (is_visible(ptr)) {
					++ptr;
				}
			}

			else if (*ptr == '"') {
				kind = SymbolKind::String;
				begin = ++ptr;

				while (*ptr != '"') {
					++ptr;
				}
			}

			else if (is_digit(ptr)) {
				kind = SymbolKind::Integer;

				while (is_digit(ptr)) {
					++ptr;
				}
			}

			else if (is_visible(ptr)) {  // Identifiers.
				kind = SymbolKind::Identifier;

				if (*ptr == '#') {
					++ptr;

					if (*ptr == '!') {
						++ptr;

						while (*ptr != '\n') {
							++ptr;
						}

						return take();
					}
				}

				while (is_visible(ptr)) {
					++ptr;
				}
			}

			else {
				fatal("unknown character");
			}

			length = ptr - begin;

			Symbol sym { { begin, length }, kind };
			Symbol out = peek;

			if (sym.kind == SymbolKind::Intrinsic) {
				if (sym.str == "decl") {
					sym.kind = SymbolKind::Declare;
				}

				else if (sym.str == "def") {
					sym.kind = SymbolKind::Label;
				}

				else if (sym.str == "addr") {
					sym.kind = SymbolKind::Address;
				}

				else {
					fatal("unknown intrinsic");
				}
			}

			peek = sym;

			return out;
		}
	};

	constexpr decltype(auto) is(SymbolKind kind) {
		return [=](Symbol other) {
			return kind == other.kind;
		};
	}

	template <typename F, typename... Ts>
	constexpr void expect(Lexer& lx, F&& fn, Ts&&... args) {
		if (not fn(lx.peek)) {
			fatal(std::forward<Ts>(args)...);
		}
	}
}  // namespace deck

// Parser
namespace deck {
	using Tree = std::vector<Symbol>;

	inline bool is_intrinsic(Symbol x) {
		return eq_any(x.kind, SymbolKind::Declare, SymbolKind::Label, SymbolKind::Address);
	}

	inline void expression(std::vector<Symbol>&, Lexer&);
	inline void frame(std::vector<Symbol>&, Lexer&);
	inline void quote(std::vector<Symbol>&, Lexer&);
	inline void intrinsic(std::vector<Symbol>&, Lexer&);

	[[nodiscard]] inline std::vector<Symbol> parse(std::string&&);

	inline void frame(std::vector<Symbol>& prog, Lexer& lx) {
		DECK_LOG(Priority::Okay);

		expect(lx, is(SymbolKind::Frame), "expected `[`");
		Symbol frame_begin = lx.take();

		prog.push_back(frame_begin);

		while (eq_none(lx.peek.kind, SymbolKind::FrameEnd, SymbolKind::Terminator)) {
			expression(prog, lx);
		}

		expect(lx, is(SymbolKind::FrameEnd), "expected `]`");
		Symbol frame_end = lx.take();

		prog.emplace_back(frame_end.str, SymbolKind::End);
	}

	inline void quote(std::vector<Symbol>& prog, Lexer& lx) {
		DECK_LOG(Priority::Okay);

		expect(lx, is(SymbolKind::Quote), "expected `{`");
		Symbol quote_begin = lx.take();

		prog.push_back(quote_begin);

		while (eq_none(lx.peek.kind, SymbolKind::QuoteEnd, SymbolKind::Terminator)) {
			expression(prog, lx);
		}

		expect(lx, is(SymbolKind::QuoteEnd), "expected `}`");
		Symbol quote_end = lx.take();

		prog.emplace_back(quote_end.str, SymbolKind::End);
	}

	inline void intrinsic(std::vector<Symbol>& prog, Lexer& lx) {
		DECK_LOG(Priority::Okay);

		expect(lx, is_intrinsic, "expected an intrinsic");
		Symbol intrinsic = lx.take();

		expect(lx, is(SymbolKind::Identifier), "expected an identifer");
		Symbol ident = lx.take();

		prog.emplace_back(ident.str, intrinsic.kind);
	}

	inline void expression(std::vector<Symbol>& prog, Lexer& lx) {
		DECK_LOG(Priority::Okay);

		switch (lx.peek.kind) {
			case SymbolKind::Frame: frame(prog, lx); break;
			case SymbolKind::Quote: quote(prog, lx); break;

			case SymbolKind::Declare:
			case SymbolKind::Label:
			case SymbolKind::Address: {
				intrinsic(prog, lx);
			} break;

			default: {
				prog.push_back(lx.take());
			};
		}
	}

	[[nodiscard]] inline std::vector<Symbol> parse(std::string&& src) {
		DECK_LOG(Priority::Okay);

		Lexer lx { std::move(src) };
		std::vector<Symbol> prog;

		prog.emplace_back(lx.peek.str, SymbolKind::Header);

		while (lx.peek.kind != SymbolKind::Terminator) {
			expression(prog, lx);
		}

		prog.emplace_back(lx.peek.str, SymbolKind::Footer);

		return prog;
	}

}  // namespace deck

// Tree utilities
namespace deck {
	template <typename F, typename... Ts>
	inline Tree::iterator visitor(const F& callback, Tree& tree, Tree::iterator it, Ts&&... args) {
		if (it == tree.end()) {
			return it;
		}

		Tree::iterator current = it++;
		callback(tree, current, it, std::forward<Ts>(args)...);

		return it;
	}

	// Visit a block (i.e. a run of code terminated by `end`).
	// This is a common pattern that shows up in most visitors that
	// traverse the AST like a tree rather than a vector.
	template <typename F, typename... Ts>
	inline Tree::iterator visit_block(F&& fn, Tree& tree, Tree::iterator it, Ts&&... args) {
		while (it != tree.end() and eq_none(it->kind, SymbolKind::End, SymbolKind::Terminator)) {
			it = visitor(fn, tree, it, std::forward<Ts>(args)...);
		}

		if (it->kind != SymbolKind::End) {
			fatal("invalid tree");
		}

		return it;
	}

	// Runs a pass by visiting every top level node until EOF is reached.
	template <typename F, typename... Ts>
	inline Tree::iterator pass(F&& fn, Tree& tree, Ts&&... args) {
		Tree::iterator it = tree.begin();

		while (it != tree.end() and it->kind != SymbolKind::Terminator) {
			it = visitor(fn, tree, it, std::forward<Ts>(args)...);
		}

		return it;
	}

}  // namespace deck

#endif
