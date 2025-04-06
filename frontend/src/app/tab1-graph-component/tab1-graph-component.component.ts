import { Component, OnInit, ElementRef, AfterViewInit, HostListener } from '@angular/core';
import * as echarts from 'echarts';

@Component({
  selector: 'app-tab1-graph',
  templateUrl: './tab1-graph-component.component.html',
  styleUrls: ['./tab1-graph-component.component.scss'],
  standalone: true,
})
export class Tab1GraphComponent implements OnInit, AfterViewInit {
  private chartInstance!: echarts.ECharts;

  constructor(private el: ElementRef) {}

  ngOnInit(): void {}

  ngAfterViewInit(): void {
    this.initChart();
    this.detectTheme(); // Applica il tema all'avvio
  }

  private initChart(): void {
    const chartElement = this.el.nativeElement.querySelector('#pieChart');
    this.chartInstance = echarts.init(chartElement);

    const chartOptions: echarts.EChartsOption = this.getChartOptions();
    this.chartInstance.setOption(chartOptions);
  }

  private getChartOptions(): echarts.EChartsOption {
    return {
      tooltip: {
        trigger: 'item',
      },
      legend: {
        orient: 'vertical', // Posizione verticale per evitare sovrapposizione
        left: 'right', // Sposta la legenda sul lato destro
        top: 'bottom', // Centra la legenda verticalmente
        textStyle: {
          color: this.isDarkMode() ? '#fff' : '#000',
        },
      },
      series: [
        {
          name: 'Devices',
          type: 'pie',
          radius: ['40%', '70%'], // Inner/outer radius per migliorare il layout
          avoidLabelOverlap: true,
          label: {
            show: true,
            position: 'outside', // Sposta le etichette fuori dal grafico
            color: this.isDarkMode() ? '#fff' : '#000',
          },
          labelLine: {
            show: true,
          },
          data: [
            { value: 40, name: 'Device A' },
            { value: 30, name: 'Device B' },
            { value: 20, name: 'Device C' },
            { value: 10, name: 'Device D' },
          ],
          emphasis: {
            itemStyle: {
              shadowBlur: 10,
              shadowOffsetX: 0,
              shadowColor: 'rgba(0, 0, 0, 0.5)',
            },
          },
        },
      ],
    };
  }  

  private isDarkMode(): boolean {
    return window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches;
  }

  private detectTheme(): void {
    const mq = window.matchMedia('(prefers-color-scheme: dark)');
    mq.addEventListener('change', () => {
      console.log('Theme changed. Updating chart...');
      this.updateChartTheme();
    });
  }

  private updateChartTheme(): void {
    if (this.chartInstance) {
      const chartOptions: echarts.EChartsOption = this.getChartOptions();
      this.chartInstance.setOption(chartOptions);
    }
  }

  @HostListener('window:resize')
  onResize(): void {
    if (this.chartInstance) {
      this.chartInstance.resize();
    }
  }
}
