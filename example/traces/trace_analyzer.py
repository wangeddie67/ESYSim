#!/usr/bin/python3

import sys
sys.path.append( '../../build' )

# load network configuration
import syscfg
netcfg_fname = '../syscfg/test.netcfg'
netcfg = syscfg.readNetCfgJson( netcfg_fname )

# load data
import trafficgen.py_esy_traffic_trace as traffictrace
file_name = '../test.trace'
max_time, trace = traffictrace.readTrafficTrace( file_name )

# analyze traffic trace
statistic_dict = traffictrace.trafficStatistic(
    trace, netcfg.niCount(), max_time )
node_series, src_vs_node, dst_vs_node = \
    traffictrace.trafficVsNode( trace, netcfg.niCount() )
node_series_str = [ str( i ) for i in node_series ]
src_vs_node_str = [ str( i ) for i in src_vs_node ]
dst_vs_node_str = [ str( i ) for i in dst_vs_node ]
time_series, src_vs_time, dst_vs_time = \
    traffictrace.trafficVsTime( trace, max_time )

# create data frame
import pandas as pd
columns = ['Time', 'Src', 'Dst', 'Length']
trace_df = pd.DataFrame( trace, columns=columns )

# Create figure
import plotly.graph_objects as go
import plotly.figure_factory as ff

# Initialize a figure with ff.create_table(table_data)
statistic_table = [ ['Parameter', 'Value', 'Unit'] ]
statistic_table.append( ['Trace file', 
                         file_name,
                         '' ] )
statistic_table.append( ['Total flits', 
                         str( statistic_dict[ 'total_flit' ] ),
                         'Flit' ] )
statistic_table.append( ['Flit injection rate', 
                         str( statistic_dict[ 'total_fir' ] ),
                         'Flit/Cycle' ] )
fig = ff.create_table( statistic_table )

# Make traces for graph
trace1 = go.Scatter(x=time_series, y=src_vs_time, xaxis='x2', yaxis='y2',
                    name='Traffic vs. Time',
                    hovertemplate="%{y} Flits<br>@Cycle %{x}<extra></extra>",
                    line={"width": 0.5},
                    mode="lines",
                    showlegend=True )

trace21 = go.Bar(x=node_series_str, y=src_vs_node_str,
                       xaxis='x3', yaxis='y3',
                    name='Traffic from Node',
                    hovertemplate="%{y} Flits<br>From Node %{x}<extra></extra>",
                    showlegend=True )
trace22 = go.Bar(x=node_series_str, y=dst_vs_node_str,
                       xaxis='x4', yaxis='y4',
                    name='Traffic to Node',
                    hovertemplate="%{y} Flits<br>To Node %{x}<extra></extra>",
                    showlegend=True )

# Add trace data to figure
fig.add_traces( [ trace1, trace21, trace22 ] )

# initialize xaxis2 and yaxis2
fig[ 'layout' ][ 'xaxis2' ] = {}
fig[ 'layout' ][ 'yaxis2' ] = {}
fig[ 'layout' ][ 'xaxis3' ] = {}
fig[ 'layout' ][ 'yaxis3' ] = {}
fig[ 'layout' ][ 'xaxis4' ] = {}
fig[ 'layout' ][ 'yaxis4' ] = {}

# Edit layout for subplots
fig.layout.xaxis.update({'domain': [0, .3]})
fig.layout.yaxis.update({'domain': [.85, 1.]})

fig.layout.xaxis2.update({'domain': [0.1, 1.0]})
fig.layout.yaxis2.update({'domain': [0.3, 0.5]})
fig.layout.yaxis2.update({'anchor': 'x2'})
fig.layout.xaxis2.update({'anchor': 'y2'})

fig.layout.xaxis3.update({'domain': [0.1, 0.5]})
fig.layout.yaxis3.update({'domain': [0.0, 0.2]})
fig.layout.yaxis3.update({'anchor': 'x3'})
fig.layout.xaxis3.update({'anchor': 'y3'})

fig.layout.xaxis4.update({'domain': [0.6, 1.0]})
fig.layout.yaxis4.update({'domain': [0.0, 0.2]})
fig.layout.yaxis4.update({'anchor': 'x4'})
fig.layout.xaxis4.update({'anchor': 'y4'})

# update subplots
fig.layout.xaxis2.update(
        rangeslider=dict(
            visible=True,
            thickness=0.04
        ),
        range=[0, max_time],
        type="linear",
        linecolor="#673ab7",
        showline=True,
        zeroline=True,
        title=dict(text='Time (cycle)',
                   standoff=0),)
fig.layout.xaxis3.update(
        #range=[-0.5, netcfg.niCount() - 0.5],
        type="linear",
        linecolor="#673ab7",
        showline=True,
        zeroline=True,
        title=dict(text='NI Id',
                   standoff=5),)
fig.layout.xaxis4.update(
        #range=[-0.5, netcfg.niCount() - 0.5],
        type="linear",
        linecolor="#673ab7",
        showline=True,
        zeroline=True,
        title=dict(text='NI Id',
                   standoff=5),)

fig.layout.yaxis2.update(
        autorange=True,
        linecolor="#673ab7",
        #range=[-60.0858369099, 28.4406294707],
        title=dict(text='Flit Count',
                   standoff=0),
        showline=True,
        side="left",
        tickfont={"color": "#673ab7"},
        tickmode="auto",
        ticks="",
        type="linear",
        zeroline=True
)
fig.layout.yaxis3.update(
        autorange=True,
        linecolor="#673ab7",
        #range=[-60.0858369099, 28.4406294707],
        title=dict(text='Flit Count',
                   standoff=0),
        showline=True,
        side="left",
        tickfont={"color": "#673ab7"},
        tickmode="auto",
        ticks="",
        type="linear",
        zeroline=True
)
fig.layout.yaxis4.update(
        autorange=True,
        linecolor="#673ab7",
        #range=[-60.0858369099, 28.4406294707],
        title=dict(text='Flit Count',
                   standoff=0),
        showline=True,
        side="left",
        tickfont={"color": "#673ab7"},
        tickmode="auto",
        ticks="",
        type="linear",
        zeroline=True
)
fig.layout.legend.update(
    x=1.0,
    xanchor='right',
    y=1.0,
    yanchor='bottom'
)

# Update the margins to add a title and see graph x-labels.
fig.layout.margin.update({'t':75})

# Update the height because adding a graph vertically will interact with
# the plot height calculated for the table
fig.layout.update( title='Traffic Trace Analysis',
                   height=1500,
                   width=1000)

# Plot!
fig.show()
