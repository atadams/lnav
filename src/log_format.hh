/**
 * Copyright (c) 2007-2012, Timothy Stack
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * * Neither the name of Timothy Stack nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @file log_format.hh
 */

#ifndef __log_format_hh
#define __log_format_hh

#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sys/types.h>

#include <string>
#include <vector>
#include <memory>

#include "pcrepp.hh"
#include "lnav_util.hh"
#include "byte_array.hh"
#include "view_curses.hh"

class logfile_filter {
public:
    typedef enum {
        MAYBE,
        INCLUDE,
        EXCLUDE,

        LFT__MAX,

        LFT__MASK = (MAYBE|INCLUDE|EXCLUDE)
    } type_t;

    logfile_filter(type_t type, std::string id)
        : lf_enabled(true),
          lf_type(type),
          lf_id(id) { };
    virtual ~logfile_filter() { };

    type_t get_type(void) const { return this->lf_type; };
    std::string get_id(void) const { return this->lf_id; };

    bool is_enabled(void) { return this->lf_enabled; };
    void enable(void) { this->lf_enabled = true; };
    void disable(void) { this->lf_enabled = false; };

    virtual bool matches(std::string line) = 0;

    virtual std::string to_command(void) = 0;

protected:
    bool        lf_enabled;
    type_t      lf_type;
    std::string lf_id;
};

typedef std::vector<logfile_filter *> filter_stack_t;

/**
 * Metadata for a single line in a log file.
 */
class logline {
public:

    /**
     * The logging level identifiers for a line(s).
     */
    typedef enum {
        LEVEL_UNKNOWN,
        LEVEL_TRACE,
        LEVEL_DEBUG,
        LEVEL_INFO,
        LEVEL_WARNING,
        LEVEL_ERROR,
        LEVEL_CRITICAL,
        LEVEL_FATAL,

        LEVEL__MAX,

        LEVEL_MULTILINE = 0x40,  /*< Start of a multiline entry. */
        LEVEL_CONTINUED = 0x80,  /*< Continuation of multiline entry. */

        /** Mask of flags for the level field. */
        LEVEL__FLAGS    = (LEVEL_MULTILINE | LEVEL_CONTINUED)
    } level_t;

    static const char *level_names[LEVEL__MAX];

    static level_t string2level(const char *levelstr, bool exact = false);

    /**
     * Construct a logline object with the given values.
     *
     * @param off The offset of the line in the file.
     * @param t The timestamp for the line.
     * @param millis The millisecond timestamp for the line.
     * @param l The logging level.
     */
    logline(off_t off,
            time_t t,
            uint16_t millis,
            level_t l,
            uint8_t m = 0)
        : ll_offset(off),
          ll_time(t),
          ll_millis(millis),
          ll_level(l),
          ll_filter_state(logfile_filter::MAYBE)
    {
        memset(this->ll_schema, 0, sizeof(this->ll_schema));
    };

    logline(off_t off,
            const struct timeval &tv,
            level_t l,
            uint8_t m = 0)
        : ll_offset(off),
          ll_level(l),
          ll_filter_state(logfile_filter::MAYBE)
    {
        this->set_time(tv);
        memset(this->ll_schema, 0, sizeof(this->ll_schema));
    };

    /** @return The offset of the line in the file. */
    off_t get_offset() const { return this->ll_offset; };

    /** @return The timestamp for the line. */
    time_t get_time() const { return this->ll_time; };

    void set_time(time_t t) { this->ll_time = t; };

    /** @return The millisecond timestamp for the line. */
    uint16_t get_millis() const { return this->ll_millis; };

    void set_millis(uint16_t m) { this->ll_millis = m; };

    struct timeval get_timeval() const {
        struct timeval retval = { this->ll_time, this->ll_millis * 1000 };

        return retval;
    };

    void set_time(const struct timeval &tv) {
        this->ll_time = tv.tv_sec;
        this->ll_millis = tv.tv_usec / 1000;
    };

    void set_multiline(void) { this->ll_level |= LEVEL_MULTILINE; };

    /** @param l The logging level. */
    void set_level(level_t l) { this->ll_level = l; };

    /** @return The logging level. */
    level_t get_level() const { return (level_t)(this->ll_level & 0xff); };

    const char *get_level_name() const
    {
        return level_names[this->ll_level & 0x0f];
    };

    bool is_continued(void) const {
        return this->get_level() & LEVEL_CONTINUED;
    };

    uint8_t get_filter_generation(void) const {
        return this->ll_filter_state >> 2;
    };

    logfile_filter::type_t get_filter_state(void) const {
        return (logfile_filter::type_t)(this->ll_filter_state &
                                        logfile_filter::LFT__MASK);
    };

    void set_filter_state(uint8_t generation, logfile_filter::type_t filter) {
        this->ll_filter_state = (generation << 2) | filter;
    };

    /**
     * @return  True if there is a schema value set for this log line.
     */
    bool has_schema(void) const
    {
        return (this->ll_schema[0] != 0 ||
                this->ll_schema[1] != 0 ||
                this->ll_schema[2] != 0 ||
                this->ll_schema[3] != 0);
    };

    /**
     * Set the "schema" for this log line.  The schema ID is used to match log
     * lines that have a similar format when generating the logline table.  The
     * schema is set lazily so that startup is faster.
     *
     * @param ba The SHA-1 hash of the constant parts of this log line.
     */
    void set_schema(const byte_array<20> &ba)
    {
        memcpy(this->ll_schema, ba.in(), sizeof(this->ll_schema));
    };

    /**
     * Perform a partial match of the given schema against this log line.
     * Storing the full schema is not practical, so we just keep the first four
     * bytes.
     *
     * @param  ba The SHA-1 hash of the constant parts of a log line.
     * @return    True if the first four bytes of the given schema match the
     *   schema stored in this log line.
     */
    bool match_schema(const byte_array<20> &ba) const
    {
        return memcmp(this->ll_schema, ba.in(), sizeof(this->ll_schema)) == 0;
    }

    /**
     * Compare loglines based on their timestamp.
     */
    bool operator<(const logline &rhs) const
    {
        return this->ll_time < rhs.ll_time ||
               (this->ll_time == rhs.ll_time &&
                this->ll_millis < rhs.ll_millis);
    };

    bool operator<(const time_t &rhs) const { return this->ll_time < rhs; };

    bool operator<(const struct timeval &rhs) const {
        return ((this->ll_time < rhs.tv_sec) ||
                (this->ll_millis < (rhs.tv_usec / 1000)));
    };

private:
    off_t    ll_offset;
    time_t   ll_time;
    uint16_t ll_millis;
    uint8_t  ll_level;
    uint8_t  ll_filter_state;
    char     ll_schema[4];
};

enum scale_op_t {
    SO_IDENTITY,
    SO_MULTIPLY,
    SO_DIVIDE
};

struct scaling_factor {
    scaling_factor() : sf_op(SO_IDENTITY), sf_value(1) { };

    template<typename T>
    void scale(T &val) const {
        switch (this->sf_op) {
        case SO_IDENTITY:
            break;
        case SO_DIVIDE:
            val = val / (T)this->sf_value;
            break;
        case SO_MULTIPLY:
            val = val * (T)this->sf_value;
            break;
        }
    }

    scale_op_t sf_op;
    double sf_value;
};

class logline_value {
public:
    enum kind_t {
        VALUE_UNKNOWN = -1,
        VALUE_TEXT,
        VALUE_INTEGER,
        VALUE_FLOAT,
    };

    static kind_t string2kind(const char *kindstr);

    logline_value(std::string name, int64_t i)
        : lv_name(name), lv_kind(VALUE_INTEGER), lv_number(i) { };
    logline_value(std::string name, double i)
        : lv_name(name), lv_kind(VALUE_FLOAT), lv_number(i) { };
    logline_value(std::string name, std::string s)
        : lv_name(name), lv_kind(VALUE_TEXT), lv_string(s) { };
    logline_value(std::string name, kind_t kind, std::string s,
                  bool ident=false, const scaling_factor *scaling=NULL)
        : lv_name(name), lv_kind(kind), lv_identifier(ident)
    {
        switch (kind) {
        case VALUE_TEXT:
            this->lv_string = s;
            break;

        case VALUE_INTEGER:
            sscanf(s.c_str(), "%" PRId64 "", &this->lv_number.i);
            if (scaling != NULL) {
                scaling->scale(this->lv_number.i);
            }
            break;

        case VALUE_FLOAT:
            sscanf(s.c_str(), "%lf", &this->lv_number.d);
            if (scaling != NULL) {
                scaling->scale(this->lv_number.d);
            }
            break;

        case VALUE_UNKNOWN:
        assert(0);
        break;
        }
    };

    const std::string to_string()
    {
        char buffer[128];

        switch (this->lv_kind) {
        case VALUE_TEXT:
            return this->lv_string;

        case VALUE_INTEGER:
            snprintf(buffer, sizeof(buffer), "%" PRId64, this->lv_number.i);
            break;

        case VALUE_FLOAT:
            snprintf(buffer, sizeof(buffer), "%lf", this->lv_number.d);
            break;
        case VALUE_UNKNOWN:
        assert(0);
        break;
        }

        return std::string(buffer);
    };

    std::string lv_name;
    kind_t      lv_kind;
    union value_u {
        int64_t i;
        double  d;

        value_u() : i(0) { };
        value_u(int64_t i) : i(i) { };
        value_u(double d) : d(d) { };
    }           lv_number;
    std::string lv_string;
    bool lv_identifier;
};

class log_vtab_impl;

/**
 * Base class for implementations of log format parsers.
 */
class log_format {
public:

    /**
     * @return The collection of builtin log formats.
     */
    static std::vector<log_format *> &get_root_formats(void);

    /**
     * Template used to register log formats during initialization.
     */
    template<class T>
    class register_root_format {
public:
        register_root_format()
        {
            static T format;

            log_format::lf_root_formats.push_back(&format);
        };
    };

    log_format() : lf_fmt_lock(-1) {
    };
    virtual ~log_format() { };

    virtual void clear(void)
    {
        this->lf_fmt_lock      = -1;
        this->lf_date_time.clear();
    };

    /**
     * Get the name of this log format.
     *
     * @return The log format name.
     */
    virtual std::string get_name(void) const = 0;

    /**
     * Scan a log line to see if it matches this log format.
     *
     * @param dst The vector of loglines that the formatter should append to
     *   if it detected a match.
     * @param offset The offset in the file where this line is located.
     * @param prefix The contents of the line.
     * @param len The length of the prefix string.
     */
    virtual bool scan(std::vector<logline> &dst,
                      off_t offset,
                      char *prefix,
                      int len) = 0;

    /**
     * Remove redundant data from the log line string.
     *
     * XXX We should probably also add some attributes to the line here, so we
     * can highlight things like the date.
     *
     * @param line The log line to edit.
     */
    virtual void scrub(std::string &line) { };

    virtual void annotate(const std::string &line,
                          string_attrs_t &sa,
                          std::vector<logline_value> &values) const
    { };

    virtual std::auto_ptr<log_format> specialized(void) = 0;

    virtual log_vtab_impl *get_vtab_impl(void) const {
        return NULL;
    };

    date_time_scanner lf_date_time;
    int lf_fmt_lock;
protected:
    static std::vector<log_format *> lf_root_formats;

    const char *log_scanf(const char *line,
                          const char *fmt[],
                          int expected_matches,
                          const char *time_fmt[],
                          char *time_dest,
                          struct tm *tm_out,
                          struct timeval &tv_out,
                          ...);
};

class external_log_format : public log_format {

public:
    struct sample {
        std::string s_line;
        logline::level_t s_level;
    };

    struct value_def {
        value_def() :
            vd_index(-1),
            vd_kind(logline_value::VALUE_UNKNOWN),
            vd_identifier(false),
            vd_foreign_key(false),
            vd_unit_field_index(-1) {

        };

        std::string vd_name;
        int vd_index;
        logline_value::kind_t vd_kind;
        std::string vd_collate;
        bool vd_identifier;
        bool vd_foreign_key;
        std::string vd_unit_field;
        int vd_unit_field_index;
        std::map<std::string, scaling_factor> vd_unit_scaling;

        bool operator<(const value_def &rhs) const {
            return this->vd_index < rhs.vd_index;
        };
    };

    struct pattern {
        pattern(const std::string &str) : p_string(str), p_pcre(NULL) { };

        std::string p_string;
        pcrepp *p_pcre;
        std::vector<value_def> p_value_by_index;
    };

    struct level_pattern {
        level_pattern() : lp_pcre(NULL) { };
        
        std::string lp_regex;
        pcrepp *lp_pcre;
    };

    external_log_format(const std::string &name) : elf_name(name) { };

    std::string get_name(void) const {
        return this->elf_name;
    };

    bool scan(std::vector<logline> &dst,
              off_t offset,
              char *prefix,
              int len);

    void annotate(const std::string &line,
                  string_attrs_t &sa,
                  std::vector<logline_value> &values) const;

    void build(std::vector<std::string> &errors);

    std::auto_ptr<log_format> specialized() {
        std::auto_ptr<log_format> retval((log_format *)
                                         new external_log_format(*this));

        retval->lf_fmt_lock = this->lf_fmt_lock;
        retval->lf_date_time = this->lf_date_time;

        return retval;
    };

    log_vtab_impl *get_vtab_impl(void) const;

    std::vector<pattern> elf_patterns;
    std::vector<sample> elf_samples;
    std::map<std::string, value_def> elf_value_defs;
    std::string elf_level_field;
    std::map<logline::level_t, level_pattern> elf_level_patterns;

private:
    const std::string elf_name;

};

#endif
