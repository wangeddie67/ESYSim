/*
 Copyright @ UESTC

 ESY-series Many-core System-on-chip Simulation Environment is free software:
 you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3
 of the License, or (at your option) any later version.

 ESY-series Many-core System-on-chip Simulation Environment is distributed in
 the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/.
 */

#ifndef INTERFACE_IODATAFILE_H
#define INTERFACE_IODATAFILE_H

#include "esy_iodatafile_item.h"
#include <vector>
#include <fstream>

using namespace std;

/** @defgroup datafileinterface Date File Interface. */

#define TOTAL_NUMBER_SPACE_TXT 30
#define TOTAL_NUMBER_SPACE_BIN sizeof(long long)

template < class ItemT >
/**
 * @ingroup datafileinterface
 * @brief Input stream to data file with format.
 *
 * Template ItemT determines the format of one item in the data file.
 *
 * Data file supports binary and text format.
 *
 * File stream holds a buffer with specified size of items in the local momery.
 * Therefore, the whole file is divided into several pages. Pages are switched
 * when the pointer reaches the start or the end of the page.
 *
 * \sa DataFormatItem, DataListItem
 */
class EsyDataFileIStream
{
protected:
	vector< ItemT > m_buf;  /**< \brief  Buffer of items in local memory.
	 *
	 * The size of buffer is determined by #m_buf_size. The items contain in
	 * the buffer forms one page. */
	long m_buf_size;  /**< \brief Size of item buffer #m_buf as well as the size
	 * of one page. */
	vector< long long int > m_pos_buf; /**< \brief Holds the position in file
	 * of items within one page. [item index] */
	size_t m_buf_p;  /**< \brief Pointer of current item in #m_buf */
    long long int m_start_pos;

	vector< long long int > m_page_pos; /**< \brief Holds the position in file
	 * of pages. [page index] */
	long m_page_p;  /**< \brief Pointer of current page in #m_page_pos */

	string m_file_name;  /**< \brief File path. */
	bool m_bin_file;  /**< \brief Binray file (TRUE) or text file (FALSE). */
	ifstream m_ifstream;  /**< \brief Input stream of data file. */
	long long m_file_size;  /**< \brief Size of data file. */
	bool m_open_success;  /**< \brief Flag of open successful. */

	string m_extension; /**< \brief Extension name. */

	long long m_total; /**< \brief 64bit total number. */

public:
	/** @name Constructor and Destructor */
	///@{
	/*!
	 * \brief Constructs an empty stream.
	 */
	EsyDataFileIStream() :
		m_buf(), m_buf_size( 10000 ), m_buf_p( 0 ), m_page_p( 0 ),
		m_file_name( "" ), m_bin_file( true ), m_open_success( false ),
		m_extension( "" ), m_total( 0 )
	{
		m_page_pos.push_back(0);
	}
	/*!
	 * \brief Constructs an input stream with specified file and buffer.
	 *
	 * Opens the data file. If the data file is open successfully, get the size
	 * of data file and load the first page of items.
	 *
	 * \param buf_size   Size of item buffer
	 * \param file_name  Direction of data file, without extension.
	 * \param bin_file   Data file is binary (true) or not (false)
	 */
	EsyDataFileIStream(
		long buf_size, const string & file_name, const string & extension, 
		bool bin_file = true
	):
		m_buf(), m_buf_size( buf_size ), m_buf_p( 0 ), m_page_p( 0 ),
		m_file_name( file_name ), m_bin_file( bin_file ),
		m_open_success( false ), m_extension( extension ), m_total( 0 )
	{
		if ( m_bin_file )
		{
			m_file_name = file_name + "." + extension + "b";
			m_ifstream.open( m_file_name.c_str(), ios::binary | ios::in );
		}
		else
		{
			m_file_name = file_name + "." + extension + "t";
			m_ifstream.open( m_file_name.c_str(), ios::in );
		}

		if ( m_ifstream.good() )
		{
			m_open_success = true;
			m_ifstream.seekg(0, ios_base::end);
			m_file_size = m_ifstream.tellg();
            m_ifstream.seekg(0, ios_base::beg);
            if ( m_bin_file )
			{
                m_ifstream.read( (char *)(&m_total), TOTAL_NUMBER_SPACE_BIN );
            }
			else
			{
                m_ifstream >> m_total;
            }

            m_start_pos = m_ifstream.tellg();
			m_page_pos.push_back( m_ifstream.tellg() );
			loadBuffer();
		}
	}
	/*!
	 * \brief Destructor.
	 *
	 * Close the input stream #m_ifstream.
	 */
	~EsyDataFileIStream()
	{
		m_ifstream.close();
	}
	///@}

	/*!
	 * \brief Get next item.
	 *
	 * If current pointer is the end of one page, load a new page before access
	 * next item.
	 * If next item is exist, return the next item. Otherwise, return an empty
	 * item.
	 *
	 * \return next item if exist or an empty item.
	 */
	ItemT getNextItem()
	{
		if ( m_buf_p == m_buf.size() )
        {
			m_page_p ++;
            if ( m_page_pos.size() <= (size_t)m_page_p )
			{
				m_page_pos.push_back( m_ifstream.tellg() );
			}
			loadBuffer();
		}

		if ( m_buf.size() > 0 )
		{
			ItemT t_item = m_buf[ m_buf_p ];
			m_buf_p ++;
			return t_item;
		}
		else
		{
			return ItemT();
		}
	}
	/*!
	 * \brief Get previous item.
	 *
	 * If current pointer is the head of one page, load a new page before access
	 * next item.
	 * If previous item is exist, return the previous item. Otherwise, return
	 * an empty item.
	 *
	 * \return previous item if exist or an empty item.
	 */
	ItemT getPrevItem()
	{
		if ( m_buf_p > 0 )
		{
			m_buf_p --;
			return m_buf[ m_buf_p ];
		}

		if ( m_page_p < 1 )
		{
			return ItemT();
		}

		m_page_p --;
		m_ifstream.clear();
		m_ifstream.seekg( m_page_pos[ m_page_p ], ios_base::beg );
		m_ifstream.clear();

		if ( loadBuffer() > 0 )
		{
			m_buf_p = m_buf.size() - 1;
			return m_buf[ m_buf_p ];
		}
		else
		{
			return ItemT();
		}
	}

	/*!
	 * \brief Close input streams.
	 */
	void close()
	{
		m_ifstream.close();
		m_open_success = false;
	}

	/*!
	 * \brief Resets input streams.
	 *
	 * Renew the pointer in file, renew the pointer of page and reload buffer.
	 */
	void reset()
	{
		m_ifstream.clear();
		m_ifstream.seekg( 0, ios_base::beg );
		if ( m_bin_file )
		{
            m_ifstream.read( (char *)(&m_total), TOTAL_NUMBER_SPACE_BIN );
		}
		else
		{
			m_ifstream >> m_total;
		}
		m_page_pos.clear();
        m_page_pos.push_back( m_ifstream.tellg() );
		m_page_p = 0;
		loadBuffer();
	}

	/*!
	 * \brief Check whether eofbit of #m_ifstream is set.
	 * \return true if the eofbit error state flag is set for the stream.
	 */
	bool eof() const { return m_ifstream.eof() && ( m_buf_p == m_buf.size() ); }
	/*!
	 * \brief Check whether the file is open successfully.
	 * \return true if the file is open without any fault bit.
	 */
	bool openSuccess() const { return m_open_success; }

	/*!
	 * \brief Access the size of file #m_file_size.
	 * \return size of file.
	 */
	long long size() const { return m_file_size; }
	/*!
	 * \brief Access the position of current item in data file.
	 * \return the position of current item in data file.
	 */
	long long pos() const { return m_pos_buf[ m_buf_p ]; }
	
	long long total() const { return m_total; }

    bool bof() { return (m_pos_buf[ m_buf_p ] <= m_start_pos); }

protected:
	/*!
	 * \brief Load item buffer.
	 *
	 * Read items in data file and fill them in the item list until the buffer
	 * is full or fault occurs.
	 *
	 * Push the start position of this page to the end of list of page position
	 * in data file #m_pos_buf.
	 *
	 * \return actual number of loaded items.
	 */
	int loadBuffer()
	{
		m_buf.clear();
		m_pos_buf.clear();
		m_buf_p = 0;

		long t_counter = 0;
		while ( m_ifstream.good() )
		{
			m_pos_buf.push_back( m_ifstream.tellg() );
			ItemT t_item;
			t_item.readItem( m_ifstream, m_bin_file );

			if ( m_ifstream.fail() )
			{
				break;
			}
			if ( m_ifstream.eof() )
			{
				break;
			}

			m_buf.push_back( t_item );

			t_counter ++;
			if ( t_counter == m_buf_size )
			{
				break;
			}
		}
		m_pos_buf.push_back( m_ifstream.tellg() );

		return t_counter;
	}
};

template < class ItemT >
/**
 * @ingroup datafileinterface
 * @brief Output stream to data file with format.
 *
 * Template ItemT determines the format of one item in the data file.
 *
 * Data file supports binary and text format. Data stream can write to a binary
 * and a text file at the same time.
 *
 * File stream holds a buffer with specified size of items in the local momery.
 * The items in the buffer flush into the file system when the buffer is full.
 *
 * \sa DataFormatItem, DataListItem
 */
class EsyDataFileOStream
{
protected:
	vector< ItemT > m_buf;  /**< \brief  Buffer of items in local memory.
	 *
	 * The size of buffer is determined by #m_buf_size. */
	size_t m_buf_size;  /**< \brief Size of item buffer #m_buf. */

	string m_bin_file_name;  /**< \brief Direction of binary data file */
	bool m_bin_enable;  /**< \brief Binary data file is enable or not. */
	ofstream m_bin_ofstream;  /**< \brief output stream to binary data file */

	string m_text_file_name;  /**< \brief Direction of text data file */
	bool m_text_enable;  /**< \brief Text data file is enable or not. */
	ofstream m_text_ofstream;  /**< \brief output stream to text data file */

	string m_extension; /**< \brief Extension name. */

	long long m_total; /**< \brief 64bit total number. */

public:
	/** @name Constructor and Destructor */
	///@{
	/*!
	 * \brief Constructs an empty stream.
	 */
	EsyDataFileOStream() :
		m_buf(), m_buf_size( 0 ), m_bin_file_name( "" ), m_bin_enable( true ),
		m_text_file_name( "" ), m_text_enable( false ), m_extension( "" ), 
		m_total( 0 )
	{}
	/*!
	 * \brief Constructs an input stream with specified file and buffer.
	 *
	 * Opens the data file.
	 *
	 * \param buf_size     Size of item buffer
	 * \param file_name    Direction of data file, without extension.
	 * \param bin_enable   Binary data file is enabled (true) or not (false)
	 * \param text_enable  Text data file is binary (true) or not (false)
	 */
	EsyDataFileOStream( 
		long buf_size, const string & file_name, const string & extension, 
		bool bin_enable = true, bool text_enable = false 
	):
		m_buf(), m_buf_size( buf_size ), m_bin_file_name( file_name ),
		m_bin_enable( bin_enable ), m_text_file_name( file_name ),
		m_text_enable( text_enable ), m_extension( extension ), m_total( 0 )
	{
		if ( bin_enable )
		{
			m_bin_file_name = file_name + "." + extension + "b";			
			m_bin_ofstream.open( m_bin_file_name.c_str(),
									ios::binary | ios::out );
		}
		if ( text_enable )
		{
			m_text_file_name = file_name + "." + extension + "t";			
			m_text_ofstream.open( m_text_file_name.c_str(), ios::out );
            for ( int i = 0; i < TOTAL_NUMBER_SPACE_TXT; i ++ )
			{
				m_text_ofstream << " ";
			}
			m_text_ofstream << endl;
		}
		writeTotal();
	}
	/*!
	 * \brief Destructor.
	 *
	 * Flush the buffer to file system. Then close the input streams.
	 */
	~EsyDataFileOStream()
	{
		flushBuffer();
		m_bin_ofstream.close();
		m_text_ofstream.close();
	}
	///@}

	/*!
	 * \brief Add next item into buffer.
	 *
	 * Add next item at the end of buffer. If the buffer is full after that,
	 * flush the buffer to the file system.
	 * \param item item to add.
	 */
	void addNextItem( const ItemT& item )
	{
		if ( m_buf_size == 0 )
		{
			ItemT t_item = item;
			if ( m_bin_enable )
			{
				t_item.writeItem( m_bin_ofstream, true );
				m_bin_ofstream.flush();
			}
			if ( m_text_enable )
			{
				t_item.writeItem( m_text_ofstream, false );
				m_text_ofstream << endl;
				m_text_ofstream.flush();
			}            
		}
		else
		{
			m_buf.push_back( item );
			if ( m_buf.size() >= m_buf_size )
			{
				flushBuffer();
			}
		}
	}

	/*!
	 * \brief Flush buffer to output streams and clear the buffer later.
	 * \return actual number of flushed items.
	 */
	int flushBuffer()
	{
		for ( size_t t_buf_p = 0; t_buf_p < m_buf.size(); t_buf_p ++ )
		{
			if ( m_bin_enable )
			{
				m_buf[ t_buf_p ].writeItem( m_bin_ofstream, true );
			}
			if ( m_text_enable )
			{
				m_buf[ t_buf_p ].writeItem( m_text_ofstream, false );
				m_text_ofstream << endl;
			}
		}
		if ( m_bin_enable )
		{
			m_bin_ofstream.flush();
		}
		if ( m_text_enable )
		{
			m_text_ofstream.flush();
		}

		vector< ItemT >().swap( m_buf );

		return (long)m_buf.size();
	}

	/*!
	 * \brief Close output streams.
	 *
	 * Flush buffer before close streams.
	 */
	void close()
	{
		flushBuffer();
		m_bin_ofstream.close();
		m_text_ofstream.close();
	}
	
	void setTotal( long long total ) { m_total = total; }
	
	void writeTotal() 
	{
		if ( m_bin_enable )
		{
			m_bin_ofstream.seekp( 0, ios_base::beg );
            m_bin_ofstream.write( (char *)(&m_total), TOTAL_NUMBER_SPACE_BIN );
			m_bin_ofstream.seekp( 0, ios_base::end );
		}
		if ( m_text_enable )
		{
			m_text_ofstream.seekp( 0, ios_base::beg );
            for ( int i = 0; i < TOTAL_NUMBER_SPACE_TXT; i ++ )
			{
				m_text_ofstream << " ";
			}
			m_text_ofstream.seekp( 0, ios_base::beg );
			m_text_ofstream << m_total;
			m_text_ofstream.seekp( 0, ios_base::end );
		}
	}
};

#endif
