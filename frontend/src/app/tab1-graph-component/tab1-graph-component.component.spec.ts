import { ComponentFixture, TestBed, waitForAsync } from '@angular/core/testing';

import { Tab1GraphComponentComponent } from './tab1-graph-component.component';

describe('Tab1GraphComponentComponent', () => {
  let component: Tab1GraphComponentComponent;
  let fixture: ComponentFixture<Tab1GraphComponentComponent>;

  beforeEach(waitForAsync(() => {
    TestBed.configureTestingModule({
      imports: [Tab1GraphComponentComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(Tab1GraphComponentComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }));

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
